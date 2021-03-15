#include "enpch.hpp"
#include "Renderer.hpp"

#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Framebuffer.hpp"

namespace Engine {

    struct Lightingdata { // holds view-space lighting data
        u32 NumPointLights;
        u32 NumSpotLights;
        Light pointLights[32];
        Light spotLights[32];
        Light sun;

        math::mat4 projection;
    };

    struct RendererData {
        std::unique_ptr<Engine::ShaderLibrary> ShaderLibrary;
        Ref<TextureCube> Skybox;

        Ref<VertexArray> FullscreenQuad;
        Ref<VertexArray> debug_coordinate_axis; //lineRender

        // Render buffers
        Ref<Framebuffer> gBuffer;
        Ref<Framebuffer> DiffuseSpecularLighting;
        Ref<Framebuffer> screenBuffer;
        //std::unordered_map<std::string, size_t> targetMap; // TODO: good idea, needs further thought

        Lightingdata Lights;

        u32 OutputMode;
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
        Renderer::GetShaderLibrary()->Load("run_tree/Data/Shaders/Lighting.glsl");
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
                FramebufferTextureFormat::RGBA16F, // View-space normal
                FramebufferTextureFormat::RGBA8, // Ambient/Metallic/Roughness
                FramebufferTextureFormat::Depth  // Depth
            };
            s_Data.gBuffer = Framebuffer::Create(spec);
        }
        {
            FramebufferSpecification spec;
            spec.Attachments = {
                FramebufferTextureFormat::RGBA8, // Diffuse
                FramebufferTextureFormat::RGBA8, // Specular
            };
            s_Data.DiffuseSpecularLighting = Framebuffer::Create(spec);
        }

        {
            FramebufferSpecification spec;
            spec.SwapChainTarget = true;
            s_Data.screenBuffer = Framebuffer::Create(spec);
        }

        s_Data.OutputMode = 0;
    }

    void Renderer::NextOutputMode() {
        s_Data.OutputMode++;
        std::vector<std::string> outputModes = {
            "Albedo",
            "View-Space Normals",
            "Baked Ambient Occlusion",
            "Metalness",
            "Roughness",
            "Ambient/Metallic/Roughness",
            "Diffuse Lighting",
            "Specular Lighting"
        };
        if (s_Data.OutputMode == outputModes.size())
            s_Data.OutputMode = 0;

        ENGINE_LOG_INFO("Output mode {1}:{0}", outputModes[s_Data.OutputMode], s_Data.OutputMode);
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

    void Renderer::UpdateLighting(const math::mat4& ViewMatrix,
        u32 numPointLights, const Light pointLights[32],
        u32 numSpotLights, const Light spotLights[32],
        const Light& sun, const math::mat4& projection) {

        s_Data.Lights.projection = projection;
        s_Data.Lights.NumPointLights = numPointLights;
        s_Data.Lights.NumSpotLights = numSpotLights;
        memcpy(&s_Data.Lights.sun, &sun, sizeof(Light));
        memcpy(s_Data.Lights.pointLights, &pointLights, sizeof(Light)*numPointLights);
        memcpy(s_Data.Lights.spotLights, &spotLights, sizeof(Light)*numSpotLights);

        math::mat4 normalMatrix = math::mat4(math::mat3(ViewMatrix)); //TODO: check the math to see if this is needed

        // recaulculate position and direction in view-space
        for (int n = 0; n < numPointLights; n++) {
            s_Data.Lights.pointLights[n].position = (ViewMatrix * math::vec4(s_Data.Lights.pointLights[n].position, 1)).XYZ();
        }
        for (int n = 0; n < numSpotLights; n++) {
            s_Data.Lights.spotLights[n].position = (ViewMatrix * math::vec4(s_Data.Lights.spotLights[n].position, 1)).XYZ();
            s_Data.Lights.spotLights[n].direction = (normalMatrix * math::vec4(s_Data.Lights.spotLights[n].direction, 0)).XYZ().get_unit();
        }
        s_Data.Lights.sun.direction = (normalMatrix * math::vec4(s_Data.Lights.sun.direction, 0)).XYZ().get_unit();
    }

    void Renderer::PrintState() {
        ENGINE_LOG_TRACE("View-space sun direction: <{0},{1},{2}>", 
            s_Data.Lights.sun.direction.x, 
            s_Data.Lights.sun.direction.y, 
            s_Data.Lights.sun.direction.z);
    }

    void Renderer::UploadLights(const Ref<Shader> shader) {
        // uploads lights to shader in view-space

        // set directional light
        shader->SetVec3("r_sun.Direction", s_Data.Lights.sun.direction);
        shader->SetVec3("r_sun.Color", s_Data.Lights.sun.color);
        shader->SetFloat("r_sun.Strength", s_Data.Lights.sun.strength);

        // set point lights
        for (int n = 0; n < s_Data.Lights.NumPointLights; n++) {
            shader->SetVec3("r_pointLights[" + std::to_string(n) + "].Position", s_Data.Lights.pointLights[n].position);
            shader->SetVec3("r_pointLights[" + std::to_string(n) + "].Color", s_Data.Lights.pointLights[n].color);
            shader->SetFloat("r_pointLights[" + std::to_string(n) + "].Strength", s_Data.Lights.pointLights[n].strength);
        }

        // set spot lights
        for (int n = 0; n < s_Data.Lights.NumSpotLights; n++) {
            shader->SetVec3("r_spotLights[" + std::to_string(n) + "].Position", s_Data.Lights.spotLights[n].position);
            shader->SetVec3("r_spotLights[" + std::to_string(n) + "].Direction", s_Data.Lights.spotLights[n].direction);
            shader->SetVec3("r_spotLights[" + std::to_string(n) + "].Color", s_Data.Lights.spotLights[n].color);
            shader->SetFloat("r_spotLights[" + std::to_string(n) + "].Strength", s_Data.Lights.spotLights[n].strength);
            shader->SetFloat("r_spotLights[" + std::to_string(n) + "].Inner", s_Data.Lights.spotLights[n].inner);
            shader->SetFloat("r_spotLights[" + std::to_string(n) + "].Outer", s_Data.Lights.spotLights[n].outer);
        }
    }

    void Renderer::Begin3DScene(const Camera& camera, const math::mat4& transform, 
        u32 numPointLights, const Light pointLights[32],
        u32 numSpotLights, const Light spotLights[32],
        const Light& sun) {

        math::mat4 ViewMatrix = math::invertViewMatrix(transform);
        math::mat4 ProjectionMatrix = camera.GetProjection();
        math::vec3 camPos = transform.col4().XYZ();
        UpdateLighting(ViewMatrix, numPointLights, pointLights, numSpotLights, spotLights, sun, ProjectionMatrix);

        s_Data.gBuffer->Bind();
        RenderCommand::SetClearColor(math::vec4(0, 0, 0, 0));
        RenderCommand::Clear();
        auto prePassShader = s_Data.ShaderLibrary->Get("PrePass");
        prePassShader->Bind();
        prePassShader->SetMat4("r_Projection", ProjectionMatrix);
        prePassShader->SetMat4("r_View", ViewMatrix);

        // set toggles
        prePassShader->SetFloat("r_AlbedoTexToggle",    1.0f);
        prePassShader->SetFloat("r_NormalTexToggle",    0.0f);
        prePassShader->SetFloat("r_MetalnessTexToggle", 1.0f);
        prePassShader->SetFloat("r_RoughnessTexToggle", 1.0f);
        prePassShader->SetFloat("r_AmbientTexToggle",   1.0f);
    }
    
    void Renderer::End3DScene() {
        s_Data.gBuffer->Unbind();

        // Lighting Pass
        s_Data.DiffuseSpecularLighting->Bind();
        auto lightingShader = s_Data.ShaderLibrary->Get("Lighting");
        lightingShader->Bind();
        lightingShader->SetInt("u_normal", 0);
        s_Data.gBuffer->BindTexture(1, 0);
        UploadLights(lightingShader);
        lightingShader->SetMat4("r_Projection", s_Data.Lights.projection);
        SubmitFullscreenQuad();
        s_Data.DiffuseSpecularLighting->Unbind();

        // Output to screen
        s_Data.screenBuffer->Bind();
        auto screenShader = s_Data.ShaderLibrary->Get("Screen");
        screenShader->Bind();
        screenShader->SetInt("u_albedo", 0);
        screenShader->SetInt("u_normal", 1);
        screenShader->SetInt("u_amr", 2);
        screenShader->SetInt("u_diffuse", 3);
        screenShader->SetInt("u_specular", 4);
        screenShader->SetInt("r_outputSwitch", s_Data.OutputMode);
        s_Data.gBuffer->BindTexture(0, 0);
        s_Data.gBuffer->BindTexture(1, 1);
        s_Data.gBuffer->BindTexture(2, 2);
        s_Data.DiffuseSpecularLighting->BindTexture(0, 3);
        s_Data.DiffuseSpecularLighting->BindTexture(1, 4);

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