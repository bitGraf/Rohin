#include <enpch.hpp>
#include "Mesh.hpp"

#include "Engine/Renderer/Renderer.hpp"

//#include "Engine/Core/DataFile.hpp"

#include "Engine/Resources/nbt/nbt.hpp"

#include "Engine/Resources/MaterialCatalog.hpp"

#include <sys/stat.h>

namespace Engine {

    // Use this moving forward
    struct Vertex
    {
        math::vec3 Position;
        math::vec3 Normal;
        math::vec3 Tangent;
        math::vec3 Bitangent;
        math::vec2 Texcoord;
    };

    // TODO: normalize this to be like a static_mesh vertex
    struct Vertex_Anim
    {
        math::vec3 Position;
        math::vec3 Normal;
        math::vec3 Tangent;
        math::vec3 Bitangent;
        math::vec2 Texcoord;
        s32 BoneIndices[4];
        math::vec4 BoneWeights;
    };

    struct Triangle
    {
        u32 V1, V2, V3;
    };

    static_assert(sizeof(Triangle) == 3 * sizeof(uint32_t));

    Mesh::~Mesh() {

    }

    Mesh::Mesh(const std::string& filename) {
        // anonymous helper funcs
        auto checkTag = [](const char* tag, const char* comp, int len) {
            bool success = true;
            for (int n = 0; n < len; n++) {
                if (comp[n] != tag[n]) {
                    success = false;
                    break;
                }
            }
            if (!success) {
                switch (len) {
                case 4: {
                    printf("ERROR::Expected '%s', got '%c%c%c%c'\n", comp, tag[0], tag[1], tag[2], tag[3]);
                    break;
                } case 8: {
                    printf("ERROR::Expected '%s', got '%c%c%c%c%c%c%c%c'\n", comp, tag[0], tag[1], tag[2], tag[3], tag[4], tag[5], tag[6], tag[7]);
                    break;
                } default: {
                    printf("ERROR::Expected '%s','\n", comp);
                    break;
                }
                }
            }
            return success;
        };
        auto read_u32 = [](std::ifstream& file) -> u32 {
            u32 res;
            file.read(reinterpret_cast<char*>(&res), sizeof(u32));
            return res;
        };
        auto read_s32 = [](std::ifstream& file) -> s32 {
            s32 res;
            file.read(reinterpret_cast<char*>(&res), sizeof(s32));
            return res;
        };
        auto read_u16 = [](std::ifstream& file) -> u16 {
            u16 res;
            file.read(reinterpret_cast<char*>(&res), sizeof(u16));
            return res;
        };
        auto read_mat4 = [](std::ifstream& file, math::mat4* m) {
            file.read(reinterpret_cast<char*>(m), 16 * sizeof(f32));
        };
        auto VERSION_CHECK = [](char vStr[4])-> bool {
            static const u8 KNOWN_VERSION_MAJOR = 0;
            static const u8 KNOWN_VERSION_MINOR = 4;
            static const u8 KNOWN_VERSION_PATCH = 2;

            if (vStr[0] != 'v') { ENGINE_LOG_ERROR("Incorrect format string read from file: '{0}' should be 'v'",vStr[0]);  return false; }
            u8 vMajor = vStr[1];
            u8 vMinor = vStr[2];
            u8 vPatch = vStr[3];

            // No backwards-compatibility rn
            if (vMajor != KNOWN_VERSION_MAJOR || vMinor != KNOWN_VERSION_MINOR || vPatch > KNOWN_VERSION_PATCH) {
                ENGINE_LOG_ERROR("Trying to load a MESH_file of version: v{0}.{1}.{2}", 
                    vMajor, vMinor, vPatch);
                ENGINE_LOG_ERROR("  Only supporting loading of versions up to v{0}.{1}.{2}",
                    KNOWN_VERSION_MAJOR, KNOWN_VERSION_MINOR, KNOWN_VERSION_PATCH);
                return false;
            }

            return true;
        };

        // Open file at the end
        std::ifstream file{ filename, std::ios::ate | std::ios::binary };

        if (!file.is_open()) {
            ENGINE_LOG_ERROR("Could not open file '{0}'!", filename);
            return;
        }

        // read current offset (at end of file) giving filesize, then go back to the start
        size_t actual_fileSize = static_cast<size_t>(file.tellg());
        file.seekg(0);

        // HEADER
        char MAGIC[4];
        file.read(MAGIC, 4);
        if (!checkTag(MAGIC, "MESH", 4)) return;

        u32 FileSize = read_u32(file);
        if (static_cast<size_t>(FileSize) != actual_fileSize) {
            ENGINE_LOG_ERROR("EROR::Expectied a fileSize of {0}, got {1}\n", actual_fileSize, FileSize);
            return;
        }

        u32 Flag = read_u32(file);

        bool has_animations = Flag & 0x01;

        char INFO[4];
        file.read(INFO, 4);
        if (!checkTag(INFO, "INFO", 4)) return;

        u32 numVerts = read_u32(file);
        u32 numInds = read_u32(file);
        u16 numSubmeshes = read_u16(file);

        char vStr[4];
        file.read(vStr, 4);
        if (!VERSION_CHECK(vStr)) return;

        u16 len = read_u16(file);
        char* comment = (char*)malloc(len);
        file.read(comment, len);
        comment[len - 1] = 0; // just in case...
        ENGINE_LOG_INFO("  Embedded comment: '{0}'", comment);
        free(comment);

        ENGINE_LOG_INFO("# of Vertices: {0}", numVerts);
        ENGINE_LOG_INFO("# of Indices:  {0}", numInds);
        ENGINE_LOG_INFO("# of Meshes:   {0}", numSubmeshes);

        m_Submeshes.resize(numSubmeshes);

        // read each submesh
        for (int n_submesh = 0; n_submesh < numSubmeshes; n_submesh++) {
            char tag[8];
            file.read(tag, 8);
            if (!checkTag(tag, "SUBMESH", 8)) return;

            auto& sm = m_Submeshes[n_submesh];
            sm.BaseIndex = read_u32(file);
            sm.MaterialIndex = read_u32(file);
            sm.IndexCount = read_u32(file);
            read_mat4(file, &sm.Transform);
        }

        // Joint heirarchy
        if (has_animations) {
            char BONE[4];
            file.read(BONE, 4);
            if (!checkTag(BONE, "BONE", 4)) return;

            u16 num_bones = read_u16(file);

            for (int n_bone = 0; n_bone < num_bones; n_bone++) {
                u16 name_len = read_u16(file);

                char* name = (char*)malloc(name_len);
                file.read(name, name_len);
                name[name_len - 1] = 0; // just in case...
                ENGINE_LOG_DEBUG("  Bone Name: '{0}'", name);
                free(name);

                s32 parent_idx = read_s32(file);

                math::vec3 local_pos;
                math::vec4 local_rot;
                math::vec3 local_scale;
                file.read(reinterpret_cast<char*>(&local_pos), 3 * sizeof(f32));
                file.read(reinterpret_cast<char*>(&local_rot), 4 * sizeof(f32));
                file.read(reinterpret_cast<char*>(&local_scale), 3 * sizeof(f32));
            }
        }

        // DATA block
        char DATA[4];
        file.read(DATA, 4);
        if (!checkTag(DATA, "DATA", 4)) return;

        // read indices
        std::vector<Triangle> m_Tris;
        m_Tris.resize(numInds * 3);
        char INDS[4];
        file.read(INDS, 4);
        if (!checkTag(INDS, "IDX", 4)) return;
        for (int n_tri = 0; n_tri < numInds / 3; n_tri++) {
            // Indices in groups of three
            m_Tris[n_tri].V1 = read_u32(file);
            m_Tris[n_tri].V2 = read_u32(file);
            m_Tris[n_tri].V3 = read_u32(file);
        }

        // read vertices
        char VERT[4];
        file.read(VERT, 4);
        if (!checkTag(VERT, "VERT", 4)) return;

        void* vertex_data_ptr = nullptr;
        size_t vertex_data_size = 0;

        if (has_animations) {
            vertex_data_size = numVerts * sizeof(Vertex_Anim);
            Vertex_Anim* m_Vertices = (Vertex_Anim*)malloc(vertex_data_size);
            vertex_data_ptr = (void*)m_Vertices;

            for (int n_vert = 0; n_vert < numVerts; n_vert++) {
                file.read(reinterpret_cast<char*>(&m_Vertices[n_vert].Position), 3 * sizeof(f32));
                file.read(reinterpret_cast<char*>(&m_Vertices[n_vert].Normal), 3 * sizeof(f32));
                file.read(reinterpret_cast<char*>(&m_Vertices[n_vert].Tangent), 3 * sizeof(f32));
                file.read(reinterpret_cast<char*>(&m_Vertices[n_vert].Bitangent), 3 * sizeof(f32));
                file.read(reinterpret_cast<char*>(&m_Vertices[n_vert].Texcoord), 2 * sizeof(f32));
                file.read(reinterpret_cast<char*>(&m_Vertices[n_vert].BoneIndices), 4 * sizeof(s32));
                file.read(reinterpret_cast<char*>(&m_Vertices[n_vert].BoneWeights), 4 * sizeof(f32));
            }
        } else {
            vertex_data_size = numVerts * sizeof(Vertex);
            Vertex* m_Vertices = (Vertex*)malloc(vertex_data_size);
            vertex_data_ptr = (void*)m_Vertices;

            for (int n_vert = 0; n_vert < numVerts; n_vert++) {
                file.read(reinterpret_cast<char*>(&m_Vertices[n_vert].Position), 3 * sizeof(f32));
                file.read(reinterpret_cast<char*>(&m_Vertices[n_vert].Normal), 3 * sizeof(f32));
                file.read(reinterpret_cast<char*>(&m_Vertices[n_vert].Tangent), 3 * sizeof(f32));
                file.read(reinterpret_cast<char*>(&m_Vertices[n_vert].Bitangent), 3 * sizeof(f32));
                file.read(reinterpret_cast<char*>(&m_Vertices[n_vert].Texcoord), 2 * sizeof(f32));
            }
        }

        // Closing tag
        char END[4];
        file.read(END, 4);

        if (END[0] != 'E' || END[1] != 'N' || END[2] != 'D' || END[3] != '\0') {
            ENGINE_LOG_ERROR("ERROR::Did not reach the END tag at the end!!\n");
            return;
        }

        // TODO: if any errrors happen, the file will not be closed properly
        file.close();

        // Set shader info
        if (has_animations) {
            m_MeshShader = Renderer::GetShaderLibrary()->Get("PrePass_Anim"); // TODO: allow meshes to choose their shader?
        } else {
            m_MeshShader = Renderer::GetShaderLibrary()->Get("PrePass"); // TODO: allow meshes to choose their shader?
        }
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

        // Create 1 material
        Ref<MaterialInstance> mat = std::make_shared<MaterialInstance>(m_BaseMaterial, "mat1");

        //mat->Set<math::vec3>("u_AlbedoColor", math::vec3(1, .5, .5));
        //mat->Set<float>("u_Metalness", 1.0f);
        //mat->Set<float>("u_Roughness", 0.75f);
        //mat->Set<float>("u_TextureScale", 2.0f);
        m_Materials.push_back(mat);

        // Data is read from file, create gpu buffers now.
        {
            m_VertexArray = VertexArray::Create();

            auto vb = VertexBuffer::Create(vertex_data_ptr, vertex_data_size);
            free(vertex_data_ptr); vertex_data_ptr = nullptr;

            if (has_animations) {
                vb->SetLayout({
                { ShaderDataType::Float3, "a_Position" },
                { ShaderDataType::Float3, "a_Normal" },
                { ShaderDataType::Float3, "a_Tangent" },
                { ShaderDataType::Float3, "a_Bitangent" },
                { ShaderDataType::Float2, "a_TexCoord" },
                { ShaderDataType::Int4,   "a_BoneIndices"},
                { ShaderDataType::Float4, "a_BoneWeights" },
                    });
            } else {
                vb->SetLayout({
                { ShaderDataType::Float3, "a_Position" },
                { ShaderDataType::Float3, "a_Normal" },
                { ShaderDataType::Float3, "a_Tangent" },
                { ShaderDataType::Float3, "a_Bitangent" },
                { ShaderDataType::Float2, "a_TexCoord" },
                    });
            }
            m_VertexArray->AddVertexBuffer(vb);

            auto ib = IndexBuffer::Create(m_Tris.data(), m_Tris.size() * 3); // TODO: make sure this is # if indices
            m_VertexArray->SetIndexBuffer(ib);

            m_VertexArray->Unbind();
        }

        m_loaded = true;
    }

    // NBT file load
    Mesh::Mesh(const std::string & filename, float, float) {
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
                { ShaderDataType::Float3, "a_Bitangent" },
                { ShaderDataType::Float2, "a_TexCoord" },
                });
            m_VertexArray->AddVertexBuffer(vb);

            auto ib = IndexBuffer::Create(m_Tris.data(), m_Tris.size() * 3); // TODO: make sure this is # if indices
            m_VertexArray->SetIndexBuffer(ib);

            m_VertexArray->Unbind();
        }

        m_loaded = true;
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
                // m_Vertices[totalIndex].Tangent = vert.tangent; //TODO: tangent 4vec -> tangent+bitangent
                m_Vertices[totalIndex].Texcoord = vert.uv;
                m_Vertices[totalIndex].BoneWeights = vert.boneWeights;
                memcpy(m_Vertices[totalIndex].BoneIndices, vert.boneIndices, 4*sizeof(vert.boneIndices[0]));
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
            
            math::mat3 BlenderCorrection(math::vec3(0, 0, 1), math::vec3(1, 0, 0), math::vec3(0, 1, 0));
            sm.Transform = math::mat4(BlenderCorrection, 1);

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
                { ShaderDataType::Float3, "a_Bitangent" },
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

    // MESH_File load
    Mesh::Mesh(const std::string & filename, int) {
        const char KNOWN_VERSION_MAJOR = 0;
        const char KNOWN_VERSION_MINOR = 2;
        const char KNOWN_VERSION_PATCH = 0;

        // anonymous helper funcs
        auto checkTag = [](const char* tag, const char* comp, int len) {
            bool success = true;
            for (int n = 0; n < len; n++) {
                if (comp[n] != tag[n]) {
                    success = false;
                    break;
                }
            }
            if (!success) {
                switch (len) {
                case 4: {
                    printf("ERROR::Expected '%s', got '%c%c%c%c'\n", comp, tag[0], tag[1], tag[2], tag[3]);
                    break;
                } case 8: {
                    printf("ERROR::Expected '%s', got '%c%c%c%c%c%c%c%c'\n", comp, tag[0], tag[1], tag[2], tag[3], tag[4], tag[5], tag[6], tag[7]);
                    break;
                } default: {
                    printf("ERROR::Expected '%s','\n", comp);
                    break;
                }
                }
            }
            return success;
        };
        auto FdGetFileSize = [](const std::string & filename) {
            struct stat stat_buf;
            int rc = stat(filename.c_str(), &stat_buf);
            return rc == 0 ? stat_buf.st_size : -1l;
        };

        long stat_filesize = FdGetFileSize(filename);

        FILE* fid = fopen(filename.c_str(), "rb");
        if (fid) {
            // HEADER
            char MAGIC[4];
            fread(MAGIC, 1, 4, fid);
            checkTag(MAGIC, "MESH", 4);

            u32 FileSize;
            fread(&FileSize, sizeof(u32), 1, fid);
            if (static_cast<long>(FileSize) != stat_filesize) printf("EROR::Expectied a fileSize of %ld, got %ld\n", stat_filesize, FileSize);

            u32 Flag;
            fread(&Flag, sizeof(u32), 1, fid);
            bool has_animations = Flag & 0x01;
            if (has_animations) {
                ENGINE_LOG_CRITICAL("Animated meshes not supported fully!!!!");
            }

            char INFO[4];
            fread(INFO, 1, 4, fid);
            checkTag(INFO, "INFO", 4);

            u32 numVerts;
            fread(&numVerts, sizeof(u32), 1, fid);

            u32 numInds;
            fread(&numInds, sizeof(u32), 1, fid);

            u16 numSubmeshes;
            fread(&numSubmeshes, sizeof(u16), 1, fid);

            char vStr[4];
            fread(vStr, sizeof(char), 4, fid);
            char vMajor = vStr[1];
            char vMinor = vStr[2];
            char vPatch = vStr[3];
            printf("File version: v%i.%i.%i\n", vMajor, vMinor, vPatch);
            printf("Highest known version is: v%i.%i.%i\n", KNOWN_VERSION_MAJOR, KNOWN_VERSION_MINOR, KNOWN_VERSION_PATCH);

            //if ( vMajor > KNOWN_VERSION_MAJOR || (vMajor <= KNOWN_VERSION_MAJOR && vMinor > KNOWN_VERSION_MINOR) ) {
            //	printf("File version too high! Highest known version is: v%i.%i.%i\n", KNOWN_VERSION_MAJOR, KNOWN_VERSION_MINOR, KNOWN_VERSION_PATCH);
            //}

            u16 len;
            fread(&len, sizeof(u16), 1, fid);
            char* comment = (char*)malloc(len);
            fread(comment, 1, len, fid);
            comment[len - 1] = 0; // just in case...
            printf("  Embedded comment: '%s'\n", comment);
            free(comment);

            printf("# of Vertices: %i\n", numVerts);
            printf("# of Indices:  %i\n", numInds);
            printf("# of Meshes:   %i\n", numSubmeshes);

            std::vector<Vertex> m_Vertices;
            m_Vertices.resize(numVerts);

            std::vector<Triangle> m_Tris;
            m_Tris.resize(numInds*3);

            m_Submeshes.resize(numSubmeshes);

            // read each submesh
            for (int n_submesh = 0; n_submesh < numSubmeshes; n_submesh++) {
                char tag[8];
                fread(tag, 1, 8, fid);
                checkTag(tag, "SUBMESH", 8);

                auto& sm = m_Submeshes[n_submesh];
                fread(&sm.BaseIndex, sizeof(u32), 1, fid);
                fread(&sm.MaterialIndex, sizeof(u32), 1, fid);
                fread(&sm.IndexCount, sizeof(u32), 1, fid);
                fread(&sm.Transform, sizeof(f32), 16, fid);
            }

            // DATA block
            char DATA[4];
            fread(DATA, 1, 4, fid);
            checkTag(DATA, "DATA", 4);

            // read vertices
            char VERT[4];
            fread(VERT, 1, 4, fid);
            checkTag(VERT, "VERT", 4);
            for (int n_vert = 0; n_vert < numVerts; n_vert++) {
                // Position
                fread(&m_Vertices[n_vert].Position, sizeof(float), 3, fid);

                // Normal
                fread(&m_Vertices[n_vert].Normal, sizeof(float), 3, fid);

                // Tangent
                fread(&m_Vertices[n_vert].Tangent, sizeof(float), 3, fid);

                // Bitangent
                fread(&m_Vertices[n_vert].Bitangent, sizeof(float), 3, fid);

                // Tex
                fread(&m_Vertices[n_vert].Texcoord, sizeof(float), 2, fid);
            }

            // read indices
            char INDS[4];
            fread(INDS, 1, 4, fid);
            checkTag(INDS, "IDX", 4);
            for (int n_tri = 0; n_tri < numInds / 3; n_tri++) {
                // Indices in groups of three
                fread(&m_Tris[n_tri], sizeof(u32), 3, fid);
            }

            // Closing tag
            char END[4];
            fread(END, 1, 4, fid);

            if (END[0] != 'E' || END[1] != 'N' || END[2] != 'D' || END[3] != '\0') {
                printf("ERROR::Did not reach the END tag at the end!!\n");
            }
            fclose(fid);

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

            // Create vertex array
            {
                m_VertexArray = VertexArray::Create();

                auto vb = VertexBuffer::Create(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex_Anim));
                vb->SetLayout({
                    { ShaderDataType::Float3, "a_Position" },
                    { ShaderDataType::Float3, "a_Normal" },
                    { ShaderDataType::Float3, "a_Tangent" },
                    { ShaderDataType::Float3, "a_Bitangent" },
                    { ShaderDataType::Float2, "a_TexCoord" },
                    { ShaderDataType::Int4,   "a_BoneIndices"},
                    { ShaderDataType::Float4, "a_BoneWeights" },
                    });
                m_VertexArray->AddVertexBuffer(vb);

                auto ib = IndexBuffer::Create(m_Tris.data(), m_Tris.size() * 3); // TODO: make sure this is # of indices
                m_VertexArray->SetIndexBuffer(ib);

                m_VertexArray->Unbind();
            }

            m_loaded = true;
        }
        else {
            printf("Failed to open file '%s'!\n", filename.c_str());
        }
    }
}