#include "enpch.hpp"
#include "Renderer.hpp"

#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Framebuffer.hpp"

namespace Engine {

    struct RendererData {
        std::unique_ptr<Engine::ShaderLibrary> ShaderLibrary;
        Ref<TextureCube> Skybox;

        Ref<VertexArray> FullscreenQuad;
        Ref<VertexArray> debug_coordinate_axis; //lineRender

        // Render buffers
        //Ref<Framebuffer> gBuffer;

        Ref<Texture2D> Albedo;
        Ref<Texture2D> ViewSpaceNormal;
        Ref<Texture2D> AmbientMetalnessRoughness;
    };

    static RendererData s_Data;

    void Renderer::Init() {
        RenderCommand::Init();

        s_Data.ShaderLibrary = std::make_unique<ShaderLibrary>();
        Renderer::GetShaderLibrary()->Load("run_tree/Data/Shaders/PBR_static.glsl");
        Renderer::GetShaderLibrary()->Load("run_tree/Data/Shaders/Skybox.glsl");
        Renderer::GetShaderLibrary()->Load("run_tree/Data/Shaders/Line.glsl");
        Renderer::GetShaderLibrary()->Load("run_tree/Data/Shaders/Normals.glsl");

        //s_Data.Skybox = TextureCube::Create("run_tree/Data/Images/DebugCubeMap.tga");
        s_Data.Skybox = TextureCube::Create("run_tree/Data/Images/snowbox.png");

        // Create fullscreen quad
        {
            float x = -1;
            float y = -1;
            float width = 2, height = 2;
            struct _vertex
            {
                math::vec3 Position;
                math::vec2 TexCoord;
            };

            _vertex* data = new _vertex[4];

            data[0].Position = math::vec3(x, y, 0.1f);
            data[0].TexCoord = math::vec2(0, 0);

            data[1].Position = math::vec3(x + width, y, 0.1f);
            data[1].TexCoord = math::vec2(1, 0);

            data[2].Position = math::vec3(x + width, y + height, 0.1f);
            data[2].TexCoord = math::vec2(1, 1);

            data[3].Position = math::vec3(x, y + height, 0.1f);
            data[3].TexCoord = math::vec2(0, 1);

            u32 indices[6] = { 0, 1, 2, 2, 3, 0 };


            auto vbo = VertexBuffer::Create(data, 4 * sizeof(_vertex));
            vbo->SetLayout({
                {ShaderDataType::Float3, "a_Position"} ,
                {ShaderDataType::Float2, "a_TexCoord"}
                });
            auto ebo = IndexBuffer::Create(indices, 6);

            s_Data.FullscreenQuad = VertexArray::Create();
            s_Data.FullscreenQuad->Bind();
            s_Data.FullscreenQuad->AddVertexBuffer(vbo);
            s_Data.FullscreenQuad->SetIndexBuffer(ebo);
            s_Data.FullscreenQuad->Unbind();
        }

        // Create debug meshes
        {
            struct _vertex
            {
                math::vec3 Position;
            };

            _vertex* data = new _vertex[6];
            float s = 0.5f;
            data[0].Position = math::vec3(-s, 0, 0);
            data[1].Position = math::vec3( s, 0, 0);
            data[2].Position = math::vec3(0, -s, 0);
            data[3].Position = math::vec3(0,  s, 0);
            data[4].Position = math::vec3(0, 0, -s);
            data[5].Position = math::vec3(0, 0,  s);

            u32 indices[6] = { 0, 1, 2, 3, 4, 5 };


            auto vbo = VertexBuffer::Create(data, 6 * sizeof(_vertex));
            vbo->SetLayout({
                { ShaderDataType::Float3, "a_Position" }
                });
            auto ebo = IndexBuffer::Create(indices, 6);

            s_Data.debug_coordinate_axis = VertexArray::Create();
            s_Data.debug_coordinate_axis->Bind();
            s_Data.debug_coordinate_axis->AddVertexBuffer(vbo);
            s_Data.debug_coordinate_axis->SetIndexBuffer(ebo);
            s_Data.debug_coordinate_axis->Unbind();
        }

        // Init text renderer
        TextRenderer::Init();

        // create the pipeline buffers
        //FramebufferSpecification spec;
        //spec.Width = 1280.0f; // TODO: This should be an application-level property
        //spec.Height = 720.0f;
        //s_Data.gBuffer = Framebuffer::Create(spec);
    }

    void Renderer::Shutdown() {
        RenderCommand::Shutdown();
    }

    void Renderer::BeginScene(const Camera& camera, const math::mat4& transform, 
        u32 numPointLights, const Light pointLights[32],
        u32 numSpotLights, const Light spotLights[32],
        const Light& sun) {

        TextRenderer::BeginScene(); // do this first to avoid context switching when rendering the 3D scene

        math::mat4 invTransform = math::invertViewMatrix(transform);
        math::mat4 viewProj = camera.GetProjection() * invTransform;
        math::mat4 invViewProj = camera.GetProjection() * transform;
        math::vec3 camPos = transform.col4().XYZ();

        auto skyboxShader = s_Data.ShaderLibrary->Get("Skybox");
        skyboxShader->Bind();
        skyboxShader->SetMat4("r_inverseVP", viewProj);
        auto samplers = skyboxShader->GetSamplers();
        for (auto& s : samplers) {
            if (s->GetName().compare("r_skybox") == 0)
                s_Data.Skybox->Bind(s->GetID());
        }
        SubmitFullscreenQuad(); // draw skybox

        auto normalsShader = s_Data.ShaderLibrary->Get("Normals");
        normalsShader->Bind();
        normalsShader->SetMat4("r_VP", viewProj);

        auto pbrShader = s_Data.ShaderLibrary->Get("PBR_static");
        pbrShader->Bind();
        pbrShader->SetMat4( "r_VP", viewProj);
        pbrShader->SetVec3( "r_CamPos", camPos);

        // set directional light
        pbrShader->SetVec3("r_sun.Direction", sun.direction);
        pbrShader->SetVec3("r_sun.Color", sun.color);
        pbrShader->SetFloat("r_sun.Strength", sun.strength);

        // set point lights
        for (int n = 0; n < numPointLights; n++) {
            pbrShader->SetVec3("r_pointLights[" + std::to_string(n) + "].Position", pointLights[n].position);
            pbrShader->SetVec3("r_pointLights[" + std::to_string(n) + "].Color", pointLights[n].color);
            pbrShader->SetFloat("r_pointLights[" + std::to_string(n) + "].Strength", pointLights[n].strength);
        }

        // set spot lights
        for (int n = 0; n < numSpotLights; n++) {
            pbrShader->SetVec3("r_spotLights[" + std::to_string(n) + "].Position",  spotLights[n].position);
            pbrShader->SetVec3("r_spotLights[" + std::to_string(n) + "].Direction", spotLights[n].direction);
            pbrShader->SetVec3("r_spotLights[" + std::to_string(n) + "].Color",     spotLights[n].color);
            pbrShader->SetFloat("r_spotLights[" + std::to_string(n) + "].Strength", spotLights[n].strength);
            pbrShader->SetFloat("r_spotLights[" + std::to_string(n) + "].Inner",    spotLights[n].inner);
            pbrShader->SetFloat("r_spotLights[" + std::to_string(n) + "].Outer",    spotLights[n].outer);
        }

        // set toggles
        pbrShader->SetFloat("r_AlbedoTexToggle",    1.0f);
        pbrShader->SetFloat("r_NormalTexToggle",    1.0f);
        pbrShader->SetFloat("r_MetalnessTexToggle", 1.0f);
        pbrShader->SetFloat("r_RoughnessTexToggle", 1.0f);
    }
    void Renderer::RenderDebug(const Camera& camera, const math::mat4& transform, 
        u32 numPointLights, const Light pointLights[32],
        u32 numSpotLights, const Light spotLights[32],
        const Light& sun) {
        math::mat4 invTransform = math::invertViewMatrix(transform);
        math::mat4 viewProj = camera.GetProjection() * invTransform;
        math::vec3 camPos = transform.col4().XYZ();

        auto lineShader = s_Data.ShaderLibrary->Get("Line");
        lineShader->Bind();
        lineShader->SetMat4("r_VP", viewProj);
        lineShader->SetVec3("r_CamPos", camPos);
        lineShader->SetFloat("r_LineFadeStart", 5);
        lineShader->SetFloat("r_LineFadeEnd", 10);

        s_Data.debug_coordinate_axis->Bind();
        for (int n = 0; n < numPointLights; n++) {
            math::mat4 pl_transform;
            pl_transform.translate(pointLights[n].position);
            lineShader->SetMat4("r_Transform", pl_transform);
            lineShader->SetVec3("r_LineColor", pointLights[n].color);
            RenderCommand::DrawLines(s_Data.debug_coordinate_axis, false);
        }
        TextRenderer::SubmitText("^_^", 50, 200, 32, { .753f,.741f,.345f });
    }
    void Renderer::EndScene() {
        TextRenderer::EndScene();// do this now to finishing rendering before 3D scene is done

        Flush();
    }

    void Renderer::Flush() {
        // currently doing nothing ^_^
    }

    void Renderer::OnWindowResize(uint32_t width, uint32_t height)
    {
        RenderCommand::SetViewport(0, 0, width, height);

        TextRenderer::OnWindowResize(width, height); // update text renderer
    }

    const std::unique_ptr<ShaderLibrary>& Renderer::GetShaderLibrary() {
        return s_Data.ShaderLibrary;
    }

    void Renderer::Submit(const math::mat4& transform) {
        auto shader = s_Data.ShaderLibrary->Get("simple");
        shader->Bind();
        shader->SetMat4("r_Transform", transform);

        //s_Data.VertexArray->Bind();
        //RenderCommand::DrawIndexed(s_Data.VertexArray);
    }

    void Renderer::SubmitMesh(const Ref<Mesh>& mesh, const math::mat4& transform) {
        mesh->GetVertexArray()->Bind();
        auto shader = mesh->GetMeshShader();
        shader->Bind();

        auto& materials = mesh->GetMaterials();
        for (Submesh& submesh : mesh->GetSubmeshes()) {
            auto material = materials[submesh.MaterialIndex];
            material->Bind();

            shader->SetMat4("r_Transform", transform * submesh.Transform);

            //RenderCommand::DrawIndexed(mesh->GetVertexArray());
            RenderCommand::DrawSubIndexed(submesh.BaseIndex, submesh.BaseVertex, submesh.IndexCount);
        }
    }

    void Renderer::SubmitMesh_drawNormals(const Ref<Mesh>& mesh, const math::mat4& transform) {
        mesh->GetVertexArray()->Bind();
        auto shader = s_Data.ShaderLibrary->Get("Normals");
        shader->Bind();

        for (Submesh& submesh : mesh->GetSubmeshes()) {
            shader->SetMat4("r_Transform", transform * submesh.Transform);

            RenderCommand::DrawSubIndexed_points(submesh.BaseIndex, submesh.BaseVertex, submesh.IndexCount);
        }
    }

    void Renderer::SubmitFullscreenQuad() {
        s_Data.FullscreenQuad->Bind();
        RenderCommand::DrawIndexed(s_Data.FullscreenQuad, false);
    }
}