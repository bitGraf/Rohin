#include <enpch.hpp>
#include "Mesh.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include "Engine/Core/DataFile.hpp"

#include "Engine/Resources/nbt/nbt.hpp"

#include "Engine/Resources/MaterialCatalog.hpp"

namespace Engine {

    Mesh::Mesh(const std::string & filename) {
        ENGINE_LOG_INFO("Loading a mesh from a .nbt file");

        nbt::file_data data;
        nbt::nbt_byte version_major, version_minor;
        endian::endian endianness;
        if (!nbt::read_from_file(filename, data, version_major, version_minor, endianness)) {
            ENGINE_LOG_ERROR("Failed to read nbt dat for mesh [{0}]", filename);
            m_loaded = false;
            return;
        }

        ENGINE_LOG_INFO("Mesh loaded correctly. Version {0}.{1}, {2}-endian", version_major, version_minor,
            (endianness == endian::big ? "big" : "little"));

        auto str = data.first;
        auto& comp = data.second->as<nbt::tag_compound>();

        int num_verts = comp["num_verts"].as<nbt::tag_int>().get();
        int num_inds  = comp["num_inds"].as<nbt::tag_int>().get();
        int num_tris = num_inds / 3;
        ENGINE_LOG_ASSERT(num_tris * 3 == num_inds, ".nbt mesh needs to be triangulated!!");
        auto vert_byte_array = comp["vertices"].as<nbt::tag_byte_array>().get();
        auto ind_int_array  = comp["indices"].as<nbt::tag_int_array>().get();

        ENGINE_LOG_ASSERT(vert_byte_array.size() == sizeof(Vertex)*num_verts, ".nbt mesh data mismatch");
        ENGINE_LOG_ASSERT(ind_int_array.size()*sizeof(nbt::nbt_int)  == sizeof(u32)*num_inds, ".nbt mesh data mismatch");

        std::vector<Vertex> m_Vertices;
        m_Vertices.reserve(num_verts);
        Vertex* _vertex = reinterpret_cast<Vertex*>(vert_byte_array.data());
        m_Vertices.assign(_vertex, _vertex + num_verts);

        std::vector<Triangle> m_Tris;
        m_Tris.reserve(num_tris);
        Triangle* _tris = reinterpret_cast<Triangle*>(ind_int_array.data());
        m_Tris.assign(_tris, _tris+ num_tris);

        /* manually fill out submesh data */
        Submesh sm;
        sm.SubmeshName = "submesh";
        sm.MaterialIndex = 0;
        sm.Transform = math::mat4();
        sm.BaseIndex = 0;
        sm.IndexCount = num_inds;
        //sm.BaseVertex = 0; // currently not using this
        m_Submeshes.push_back(sm);

        // Set shader info
        m_MeshShader = Renderer::GetShaderLibrary()->Get("PrePass"); // TODO: allow meshes to choose their shader?
        m_BaseMaterial = std::make_shared<Material>(m_MeshShader);

        // Manually set material struct
        {
            MaterialSpec mat_spec;
            // see if a comprehensive material is listed to load
            if (comp.has_key("material")) {
                // load material props from this material listing
                const auto& material_name = comp["material"].as<nbt::tag_string>().get();
                mat_spec = MaterialCatalog::GetMaterial(material_name);
                ENGINE_LOG_TRACE("This mesh is using material {0}[{1}]", material_name, mat_spec.Name);
            }

            // If mesh nbt file overrwrites anything, capture that
            mat_spec.AlbedoBase = nbt::SafeGetVec3(comp, "albedo_color", mat_spec.AlbedoBase);
            mat_spec.MetalnessBase = nbt::SafeGetFloat(comp, "metalness", mat_spec.MetalnessBase);
            mat_spec.RoughnessBase = nbt::SafeGetFloat(comp, "roughness", mat_spec.RoughnessBase);
            mat_spec.TextureScale = nbt::SafeGetFloat(comp, "texture_scale", mat_spec.TextureScale);

            if (comp.has_key("albedo_path")) {
                const auto& albedo_path = comp.at("albedo_path").as<nbt::tag_string>().get();
                mat_spec.Albedo = MaterialCatalog::GetTexture(albedo_path);
            }
            if (comp.has_key("normal_path")) {
                const auto& normal_path = comp.at("normal_path").as<nbt::tag_string>().get();
                mat_spec.Normal = MaterialCatalog::GetTexture(normal_path);
            }
            if (comp.has_key("ambient_path")) {
                const auto& ambient_path = comp.at("ambient_path").as<nbt::tag_string>().get();
                mat_spec.Ambient = MaterialCatalog::GetTexture(ambient_path);
            }
            if (comp.has_key("metalness_path")) {
                const auto& metalness_path = comp.at("metalness_path").as<nbt::tag_string>().get();
                mat_spec.Metalness = MaterialCatalog::GetTexture(metalness_path);
            }
            if (comp.has_key("roughness_path")) {
                const auto& roughness_path = comp.at("roughness_path").as<nbt::tag_string>().get();
                mat_spec.Roughness = MaterialCatalog::GetTexture(roughness_path);
            }
            if (comp.has_key("emissive_path")) {
                const auto& emissive_path = comp.at("emissive_path").as<nbt::tag_string>().get();
                mat_spec.Emissive = MaterialCatalog::GetTexture(emissive_path);
            }
                    
            // If after the material and texture definitions, some channels are still
            // not set, set them to the default texture values
            if (!mat_spec.Albedo)    mat_spec.Albedo    = MaterialCatalog::GetTexture("Data/Images/frog.png");
            if (!mat_spec.Normal)    mat_spec.Normal    = MaterialCatalog::GetTexture("Data/Images/normal.png");
            if (!mat_spec.Ambient)   mat_spec.Ambient   = MaterialCatalog::GetTexture("Data/Images/white.png");
            if (!mat_spec.Metalness) mat_spec.Metalness = MaterialCatalog::GetTexture("Data/Images/black.png");
            if (!mat_spec.Roughness) mat_spec.Roughness = MaterialCatalog::GetTexture("Data/Images/white.png");
            if (!mat_spec.Emissive)  mat_spec.Emissive  = MaterialCatalog::GetTexture("Data/Images/black.png");

            // print out texture IDs
            //ENGINE_LOG_INFO("Mesh {0} has texIDs ({1},{2},{3},{4},{5},{6})", filename, mat_spec.Albedo->GetID(), mat_spec.Normal->GetID(), mat_spec.Ambient->GetID(), mat_spec.Metalness->GetID(), mat_spec.Albedo->GetID(), mat_spec.Roughness->GetID());

            // mat_spec should now have all the valid data needed!
            // upload everyhing from mat_spec to the material
            m_BaseMaterial->Set<math::vec3>("u_AlbedoColor", mat_spec.AlbedoBase);
            m_BaseMaterial->Set<float>("u_Metalness", mat_spec.MetalnessBase);
            m_BaseMaterial->Set<float>("u_Roughness", mat_spec.RoughnessBase);
            m_BaseMaterial->Set<float>("u_TextureScale", mat_spec.TextureScale);

            m_BaseMaterial->Set("u_AlbedoTexture",    mat_spec.Albedo);
            m_BaseMaterial->Set("u_NormalTexture",    mat_spec.Normal);
            m_BaseMaterial->Set("u_MetalnessTexture", mat_spec.Metalness);
            m_BaseMaterial->Set("u_RoughnessTexture", mat_spec.Roughness);
            m_BaseMaterial->Set("u_AmbientTexture",   mat_spec.Ambient);
            m_BaseMaterial->Set("u_EmissiveTexture",  mat_spec.Emissive);

            /// mat1
            Ref<MaterialInstance> mat = std::make_shared<MaterialInstance>(m_BaseMaterial, "mat1");

            //mat->Set<math::vec3>("u_AlbedoColor", math::vec3(1, .5, .5));
            //mat->Set<float>("u_Metalness", 1.0f);
            //mat->Set<float>("u_Roughness", 0.75f);
            //mat->Set<float>("u_TextureScale", 2.0f);
            m_Materials.push_back(mat);
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

            auto ib = IndexBuffer::Create(m_Tris.data(), m_Tris.size() * 3); // TODO: make sure this is # if indices
            m_VertexArray->SetIndexBuffer(ib);

            m_VertexArray->Unbind();
        }

        m_loaded = true;
    }
    
    Mesh::~Mesh() {

    }

    void Mesh::OnUpdate(double ts) {

    }

    void Mesh::DumpVertexBuffer() {

    }


    // MD5 conversion
    Mesh::Mesh(const md5::Model& model) {
        // combine all md5 meshes verts into one vertex buffer
        int totalVerts = 0;
        int totalTris = 0;
        std::vector<int> mesh_vert_offsets;
        std::vector<int> mesh_tri_offsets;
        std::unordered_map<std::string, int> materialIndexMap;
        int numMats = 0;
        for (int n = 0; n < model.Meshes.size(); n++) {
            mesh_vert_offsets.push_back(totalVerts);
            totalVerts += model.Meshes[n].Verts.size();

            mesh_tri_offsets.push_back(totalTris);
            totalTris += model.Meshes[n].Tris.size();

            const auto& mesh = model.Meshes[n];
            if (materialIndexMap.find(mesh.Shader) == materialIndexMap.end()) {
                materialIndexMap.emplace(mesh.Shader, numMats);
                numMats++;
            }
        }

        std::vector<Vertex_Anim> m_Vertices;
        m_Vertices.resize(totalVerts);

        std::vector<Triangle> m_Tris;
        m_Tris.resize(totalTris);

        // for every md5::mesh
        for (int m = 0; m < model.Meshes.size(); m++) {
            const md5::Mesh& mesh = model.Meshes[m];

            // add verts to total array
            for (int v = 0; v < mesh.Verts.size(); v++) {
                const md5::Vert& vert = mesh.Verts[v];

                int totalIndex = v + mesh_vert_offsets[m];
                m_Vertices[totalIndex].Position = vert.position;
                m_Vertices[totalIndex].Normal = vert.normal;
                //m_Vertices[totalIndex].Tangent = vert.position;
                //m_Vertices[totalIndex].Binormal = vert.position;
                m_Vertices[totalIndex].Texcoord = vert.uv;

                s32 boneIdx[4] = { 0,0,0,0 };
                f32 boneWeight[4] = { 0,0,0,0 };
                for (int w = 0; w < vert.countWeight; w++) {
                    boneIdx[w] = mesh.Weights[vert.startWeight + w].joint;
                    boneWeight[w] = mesh.Weights[vert.startWeight + w].bias;
                }
                memcpy(&m_Vertices[totalIndex].BoneIndices[0], boneIdx,    4 * sizeof(s32));
                memcpy(&m_Vertices[totalIndex].BoneWeights.x,  boneWeight, 4 * sizeof(f32));
            }

            // add indices to total array
            for (int t = 0; t < mesh.Tris.size(); t++) {
                const md5::Tri& tri = mesh.Tris[t];

                int totalIndex = t + mesh_tri_offsets[m];
                // MD5Mesh has the opposite triangle ordering as us, so we sample out of order here
                m_Tris[totalIndex].V1 = tri.vertIndex[0] + mesh_vert_offsets[m];
                m_Tris[totalIndex].V2 = tri.vertIndex[2] + mesh_vert_offsets[m];
                m_Tris[totalIndex].V3 = tri.vertIndex[1] + mesh_vert_offsets[m];
            }

            // fill out submesh data
            Submesh sm;
            
            //sm.BaseVertex = mesh_vert_offsets[m];
            sm.BaseIndex = mesh_tri_offsets[m] * 3;
            sm.MaterialIndex = materialIndexMap[mesh.Shader];
            sm.IndexCount = mesh.Tris.size() * 3;
            
            math::mat3 rotM;
            math::CreateRotationFromYawPitch(rotM, 180, -90);
            math::CreateTransform(sm.Transform, rotM, math::vec3(0,0,0), math::vec3(.3f));
            //sm.Transform = math::createYawPitchRollMatrix(180, 0, -90);
            //sm.Transform.scale({ .3f,.3f,.3f });

            sm.SubmeshName = mesh.Shader; // TODO: hehe

            m_Submeshes.push_back(sm);
        }

        // Save bind pose info
        m_BindPose = model.Joints;

        // Set shader info
        m_MeshShader = Renderer::GetShaderLibrary()->Get("PrePass_Anim"); // TODO: allow meshes to choose their shader?
        m_BaseMaterial = std::make_shared<Material>(m_MeshShader);

        // Create materials
        // Manually set material struct
        MaterialSpec mat_spec;

        // If after the material and texture definitions, some channels are still
        // not set, set them to the default texture values
        mat_spec.Albedo = MaterialCatalog::GetTexture("Data/Images/frog.png");
        mat_spec.Normal = MaterialCatalog::GetTexture("Data/Images/normal.png");
        mat_spec.Ambient = MaterialCatalog::GetTexture("Data/Images/white.png");
        mat_spec.Metalness = MaterialCatalog::GetTexture("Data/Images/black.png");
        mat_spec.Roughness = MaterialCatalog::GetTexture("Data/Images/white.png");
        mat_spec.Emissive = MaterialCatalog::GetTexture("Data/Images/black.png");

        // mat_spec should now have all the valid data needed!
        // upload everyhing from mat_spec to the material
        m_BaseMaterial->Set<math::vec3>("u_AlbedoColor", mat_spec.AlbedoBase);
        m_BaseMaterial->Set<float>("u_Metalness", mat_spec.MetalnessBase);
        m_BaseMaterial->Set<float>("u_Roughness", mat_spec.RoughnessBase);
        m_BaseMaterial->Set<float>("u_TextureScale", mat_spec.TextureScale);

        m_BaseMaterial->Set("u_AlbedoTexture", mat_spec.Albedo);
        m_BaseMaterial->Set("u_NormalTexture", mat_spec.Normal);
        m_BaseMaterial->Set("u_MetalnessTexture", mat_spec.Metalness);
        m_BaseMaterial->Set("u_RoughnessTexture", mat_spec.Roughness);
        m_BaseMaterial->Set("u_AmbientTexture", mat_spec.Ambient);
        m_BaseMaterial->Set("u_EmissiveTexture", mat_spec.Emissive);

        m_Materials.resize(numMats);
        for (const auto ent : materialIndexMap) {
            auto spec = MaterialCatalog::GetMaterial(ent.first);

            auto& mat = m_Materials[ent.second];
            mat = std::make_shared<MaterialInstance>(m_BaseMaterial, ent.first);

            //mat->Set<math::vec3>("u_AlbedoColor", spec.AlbedoBase);
            //mat->Set<float>("u_Metalness", spec.MetalnessBase);
            //mat->Set<float>("u_Roughness", spec.RoughnessBase);
            //mat->Set<float>("u_TextureScale", spec.TextureScale);

            mat->Set("u_AlbedoTexture", spec.Albedo);
            mat->Set("u_NormalTexture", spec.Normal);
            //mat->Set("u_MetalnessTexture", spec.Metalness);
            //mat->Set("u_RoughnessTexture", spec.Roughness);
            //mat->Set("u_AmbientTexture", spec.Ambient);
            //mat->Set("u_EmissiveTexture", spec.Emissive);
        }

        // Create vertex array
        {
            m_VertexArray = VertexArray::Create();

            auto vb = VertexBuffer::Create(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex_Anim));
            vb->SetLayout({
                { ShaderDataType::Float3, "a_Position" },
                { ShaderDataType::Float3, "a_Normal" },
                { ShaderDataType::Float3, "a_Tangent" },
                { ShaderDataType::Float3, "a_Binormal" },
                { ShaderDataType::Float2, "a_TexCoord" },
                { ShaderDataType::Int4,   "a_BoneIndices"},
                { ShaderDataType::Float4, "a_BoneWeights" },
                });
            m_VertexArray->AddVertexBuffer(vb);

            auto ib = IndexBuffer::Create(m_Tris.data(), m_Tris.size() * 3); // TODO: make sure this is # if indices
            m_VertexArray->SetIndexBuffer(ib);

            m_VertexArray->Unbind();
        }

        m_loaded = true;
    }
}