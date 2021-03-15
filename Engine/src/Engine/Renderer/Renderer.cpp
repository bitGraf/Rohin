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
        Ref<Framebuffer> gBuffer;
        Ref<Framebuffer> screenBuffer;
        std::unordered_map<std::string, size_t> targetMap;
    };

    static RendererData s_Data;

    void Renderer::Init() {
        RenderCommand::Init();

        s_Data.ShaderLibrary = std::make_unique<ShaderLibrary>();
        Renderer::GetShaderLibrary()->Load("run_tree/Data/Shaders/PBR_static.glsl");
        Renderer::GetShaderLibrary()->Load("run_tree/Data/Shaders/Skybox.glsl");
        Renderer::GetShaderLibrary()->Load("run_tree/Data/Shaders/Line.glsl");
        Renderer::GetShaderLibrary()->Load("run_tree/Data/Shaders/Normals.glsl");

        Renderer::GetShaderLibrary()->Load("run_tree/Data/Shaders/PrePass.glsl");
        Renderer::GetShaderLibrary()->Load("run_tree/Data/Shaders/Screen.glsl");

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

        // create the pipeline buffers
        {
            FramebufferSpecification spec;
            spec.Attachments = {
                FramebufferTextureFormat::RGBA8, // Albedo
                FramebufferTextureFormat::RGBA8, // View-space normal
                FramebufferTextureFormat::RGBA8, // Ambient/Metallic/Roughness
                FramebufferTextureFormat::Depth  // Depth
            };
            s_Data.gBuffer = Framebuffer::Create(spec);
            s_Data.targetMap["Albedo"] = 0;
            s_Data.targetMap["Normal"] = 1;
            s_Data.targetMap["AMR"] = 2;
        }

        {
            FramebufferSpecification spec;
            spec.SwapChainTarget = true;
            s_Data.screenBuffer = Framebuffer::Create(spec);
        }
    }

    void Renderer::Shutdown() {
        RenderCommand::Shutdown();
    }


    void DrawSkybox() {
        /*
        // TODO: make this work with a deferred renderer :(
        auto skyboxShader = s_Data.ShaderLibrary->Get("Skybox");
        skyboxShader->Bind();
        skyboxShader->SetMat4("r_inverseVP", viewProj);
        auto samplers = skyboxShader->GetSamplers();
        for (auto& s : samplers) {
        if (s->GetName().compare("r_skybox") == 0)
        s_Data.Skybox->Bind(s->GetID());
        }
        SubmitFullscreenQuad(); // draw skybox
        */
    }

    void LightingPass() {
        /*
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
            pbrShader->SetVec3("r_spotLights[" + std::to_string(n) + "].Position", spotLights[n].position);
            pbrShader->SetVec3("r_spotLights[" + std::to_string(n) + "].Direction", spotLights[n].direction);
            pbrShader->SetVec3("r_spotLights[" + std::to_string(n) + "].Color", spotLights[n].color);
            pbrShader->SetFloat("r_spotLights[" + std::to_string(n) + "].Strength", spotLights[n].strength);
            pbrShader->SetFloat("r_spotLights[" + std::to_string(n) + "].Inner", spotLights[n].inner);
            pbrShader->SetFloat("r_spotLights[" + std::to_string(n) + "].Outer", spotLights[n].outer);
        }
        */
    }

    void Renderer::Begin3DScene(const Camera& camera, const math::mat4& transform, 
        u32 numPointLights, const Light pointLights[32],
        u32 numSpotLights, const Light spotLights[32],
        const Light& sun) {

        s_Data.gBuffer->Bind();
        RenderCommand::Clear();
        math::mat4 ViewMatrix = math::invertViewMatrix(transform);
        math::mat4 ProjectionMatrix = camera.GetProjection();
        math::vec3 camPos = transform.col4().XYZ();

        auto prePassShader = s_Data.ShaderLibrary->Get("PrePass");
        prePassShader->Bind();
        prePassShader->SetMat4("r_View", ViewMatrix);
        prePassShader->SetMat4("r_Projection", ProjectionMatrix);

        // set toggles
        prePassShader->SetFloat("r_AlbedoTexToggle",    1.0f);
        prePassShader->SetFloat("r_NormalTexToggle",    1.0f);
        prePassShader->SetFloat("r_MetalnessTexToggle", 1.0f);
        prePassShader->SetFloat("r_RoughnessTexToggle", 1.0f);
        prePassShader->SetFloat("r_AmbientTexToggle",   1.0f);
    }
    
    void Renderer::End3DScene() {
        s_Data.gBuffer->Unbind();

        // Lighting Pass
        s_Data.screenBuffer->Bind();
        auto screenShader = s_Data.ShaderLibrary->Get("Screen");
        screenShader->Bind();
        screenShader->SetInt("u_tex1", 0);
        screenShader->SetInt("u_tex2", 1);
        screenShader->SetInt("u_tex3", 2);
        s_Data.gBuffer->BindTexture(0, 0);
        s_Data.gBuffer->BindTexture(1, 1);
        s_Data.gBuffer->BindTexture(2, 2);

        SubmitFullscreenQuad();
        s_Data.screenBuffer->Unbind();

        Flush();
    }

    void Renderer::Flush() {
        // currently doing nothing ^_^
    }

    void Renderer::OnWindowResize(uint32_t width, uint32_t height)
    {
        RenderCommand::SetViewport(0, 0, width, height);
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