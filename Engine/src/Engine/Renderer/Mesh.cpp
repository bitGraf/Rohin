#include <enpch.hpp>
#include "Mesh.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include "Engine/Core/DataFile.hpp"

namespace Engine {

    Mesh::Mesh(const std::string & filename, bool mesh_only) : m_FilePath(filename) {
        ENGINE_LOG_INFO("Loading mesh: {0}", filename);

        u32 highestMaterialIndex = 0;
        DataFile file;
        file.ReadFromFile(filename);

        // Load Meshes
        {
            auto& meshBlock = file.GetBlock("Mesh");
            m_Name = meshBlock.read_string();

            auto numSubmeshes = meshBlock.read_byte();
            auto numVerts = meshBlock.read_short();
            auto numInds = meshBlock.read_short();
            
            Vertex* __vertices_ = new Vertex[numVerts];
            meshBlock.read_data(reinterpret_cast<u8*>(__vertices_), numVerts * sizeof(Vertex));
            m_Vertices.reserve(numVerts);
            m_Vertices.assign(__vertices_, __vertices_ + numVerts);
            delete[] __vertices_;

            u32* __indices_ = new u32[numInds];
            meshBlock.read_data(reinterpret_cast<u8*>(__indices_), numInds * sizeof(u32));
            m_Indices.resize(numInds / 3);
            int i = 0;
            for (int n = 0; n < numInds / 3; n++) {
                m_Indices[n].V1 = __indices_[i++];
                m_Indices[n].V2 = __indices_[i++];
                m_Indices[n].V3 = __indices_[i++];
            }
            delete[] __indices_;

            auto meshFlag = meshBlock.read_byte();

            // load submeshes
            for (int n = 0; n < numSubmeshes; n++) {
                Submesh sm;
                sm.SubmeshName = meshBlock.read_string();
                sm.MaterialIndex = meshBlock.read_byte();
                sm.Transform = meshBlock.read<math::mat4>();
                sm.BaseIndex = meshBlock.read_short();
                sm.IndexCount = meshBlock.read_short();
                sm.BaseVertex = 0; // currently not using this
                m_Submeshes.push_back(sm);

                if (sm.MaterialIndex > highestMaterialIndex)
                    highestMaterialIndex = sm.MaterialIndex;
            }
        }
        
        if (mesh_only)
            return;

        // Set shader info
        m_MeshShader = Renderer::GetShaderLibrary()->Get("PBR_static");
        m_BaseMaterial = std::make_shared<Material>(m_MeshShader);

        // Load Materials
        {
            auto& matBlock = file.GetBlock("Materials");

            // base material properties
            m_BaseMaterial->Set<math::vec3>("u_AlbedoColor", matBlock.read<math::vec3>());
            m_BaseMaterial->Set<float>("u_Metalness", matBlock.read<float>());
            m_BaseMaterial->Set<float>("u_Roughness", matBlock.read<float>());

            auto numTextures_base = matBlock.read_byte();
            for (int n = 0; n < numTextures_base; n++) {
                auto texName = matBlock.read_string();
                auto texPath = matBlock.read_string();

                auto tex = Texture2D::Create(texPath);
                m_BaseMaterial->Set(texName, tex);
            }

            auto numMats = matBlock.read_byte();
            for (int n = 0; n < numMats; n++) {
                auto matName = matBlock.read_string();
                Ref<MaterialInstance> mat = std::make_shared<MaterialInstance>(m_BaseMaterial, matName);

                m_BaseMaterial->Set<math::vec3>("u_AlbedoColor", matBlock.read<math::vec3>());
                m_BaseMaterial->Set<float>("u_Metalness", matBlock.read<float>());
                m_BaseMaterial->Set<float>("u_Roughness", matBlock.read<float>());

                auto numTextures = matBlock.read_byte();
                for (int n = 0; n < numTextures; n++) {
                    auto texName = matBlock.read_string();
                    auto texPath = matBlock.read_string();

                    auto tex = Texture2D::Create(texPath); //TODO: make sure we are not loading multiple copies of same textue. TextureLibrary?
                    mat->Set(texName, tex);
                }
                
                m_Materials.push_back(mat);
            }
        }

        // Create vertex array
        {
            m_VertexArray = VertexArray::Create();

            auto vb = VertexBuffer::Create(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));
            vb->SetLayout({
                { ShaderDataType::Float3, "a_Position" },
                { ShaderDataType::Float3, "a_Normal" },
                { ShaderDataType::Float3, "a_Tangent" },
                { ShaderDataType::Float3, "a_Binormal" },
                { ShaderDataType::Float2, "a_TexCoord" },
            });
            m_VertexArray->AddVertexBuffer(vb);

            auto ib = IndexBuffer::Create(m_Indices.data(), m_Indices.size() * sizeof(Index));
            m_VertexArray->SetIndexBuffer(ib);
        }
    }
    
    Mesh::~Mesh() {

    }

    void Mesh::OnUpdate(double ts) {

    }

    void Mesh::DumpVertexBuffer() {

    }
}