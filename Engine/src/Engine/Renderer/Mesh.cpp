#include <enpch.hpp>
#include "Mesh.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include "Engine/Core/DataFile.hpp"

#include "Engine/Resources/nbt/nbt.hpp"

#include "Engine/Resources/MaterialCatalog.hpp"

namespace Engine {

    Mesh::Mesh(const std::string & filename, bool nbt, bool mesh_only) : m_FilePath(filename) {
        if (nbt) {
            ENGINE_LOG_INFO("Loading a mesh from a .nbt file");

            nbt::file_data data;
            nbt::nbt_byte version_major, version_minor;
            endian::endian endianness;
            auto result = nbt::read_from_file(filename, data, version_major, version_minor, endianness);

            if (result) {
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

                m_Vertices.reserve(num_verts);
                Vertex* _vertex = reinterpret_cast<Vertex*>(vert_byte_array.data());
                m_Vertices.assign(_vertex, _vertex + num_verts);

                m_Indices.reserve(num_tris);
                Index* _index = reinterpret_cast<Index*>(ind_int_array.data());
                m_Indices.assign(_index, _index + num_tris);

                for (auto tri : m_Indices) {
                    auto v1 = m_Vertices[tri.V1];
                    auto v2 = m_Vertices[tri.V2];
                    auto v3 = m_Vertices[tri.V3];

                    bool ddd = false;
                }

                /* manually fill out submesh data */
                Submesh sm;
                sm.SubmeshName = "submesh";
                sm.MaterialIndex = 0;
                sm.Transform = math::mat4();
                sm.BaseIndex = 0;
                sm.IndexCount = num_inds;
                sm.BaseVertex = 0; // currently not using this
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
                    if (!mat_spec.Albedo)    mat_spec.Albedo    = MaterialCatalog::GetTexture("run_tree/Data/Images/frog.png");
                    if (!mat_spec.Normal)    mat_spec.Normal    = MaterialCatalog::GetTexture("run_tree/Data/Images/normal.png");
                    if (!mat_spec.Ambient)   mat_spec.Ambient   = MaterialCatalog::GetTexture("run_tree/Data/Images/white.png");
                    if (!mat_spec.Metalness) mat_spec.Metalness = MaterialCatalog::GetTexture("run_tree/Data/Images/black.png");
                    if (!mat_spec.Roughness) mat_spec.Roughness = MaterialCatalog::GetTexture("run_tree/Data/Images/white.png");
                    if (!mat_spec.Emissive)  mat_spec.Emissive  = MaterialCatalog::GetTexture("run_tree/Data/Images/black.png");

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

                    auto ib = IndexBuffer::Create(m_Indices.data(), m_Indices.size() * 3); // TODO: make sure this is # if indices
                    m_VertexArray->SetIndexBuffer(ib);
                }

                bool done = true;
            }
        }
        else {
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
                        highestMaterialIndex = sm.MaterialIndex; //TODO: why is this here?
                }
            }

            if (mesh_only)
                return;

            // Set shader info
            m_MeshShader = Renderer::GetShaderLibrary()->Get("PBR_static"); // TODO: allow meshes to choose their shader?
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

                    mat->Set<math::vec3>("u_AlbedoColor", matBlock.read<math::vec3>());
                    mat->Set<float>("u_Metalness", matBlock.read<float>());
                    mat->Set<float>("u_Roughness", matBlock.read<float>());

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

                auto ib = IndexBuffer::Create(m_Indices.data(), m_Indices.size() * 3); // TODO: make sure this is # if indices
                m_VertexArray->SetIndexBuffer(ib);
            }
        }

        m_VertexArray->Unbind();
    }
    
    Mesh::~Mesh() {

    }

    void Mesh::OnUpdate(double ts) {

    }

    void Mesh::DumpVertexBuffer() {

    }
}