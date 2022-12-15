#include <enpch.hpp>
#include "Mesh.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include "Engine/Resources/nbt/nbt.hpp"

#include "Engine/Resources/MaterialCatalog.hpp"

namespace rh {

    static_assert(sizeof(Triangle) == 3 * sizeof(uint32_t));

    static const u8 KNOWN_VERSION_MAJOR = 1;
    static const u8 KNOWN_VERSION_MINOR = 0;
    static const u8 KNOWN_VERSION_PATCH = 0;

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
    auto read_f32 = [](std::ifstream& file) -> f32 {
        f32 res;
        file.read(reinterpret_cast<char*>(&res), sizeof(f32));
        return res;
    };
    auto read_f64 = [](std::ifstream& file) -> f64 {
        f64 res;
        file.read(reinterpret_cast<char*>(&res), sizeof(f64));
        return res;
    };
    auto read_mat4 = [](std::ifstream& file, laml::Mat4* m) {
        file.read(reinterpret_cast<char*>(m), 16 * sizeof(f32));
    };
    auto read_quat = [](std::ifstream& file, laml::Quat* q) {
        file.read(reinterpret_cast<char*>(q), 4 * sizeof(f32));
    };
    auto VERSION_CHECK = [](char vStr[4])-> bool {
        if (vStr[0] != 'v') { ENGINE_LOG_ERROR("Incorrect format string read from file: '{0}' should be 'v'", vStr[0]);  return false; }
        u8 vMajor = vStr[1];
        u8 vMinor = vStr[2];
        u8 vPatch = vStr[3];

        // No backwards-compatibility rn
        if (vMajor != KNOWN_VERSION_MAJOR || vMinor != KNOWN_VERSION_MINOR) {
            ENGINE_LOG_ERROR("Trying to load a MESH_file of version: v{0}.{1}.   Supported version: v{2}.{3}",
                vMajor, vMinor, KNOWN_VERSION_MAJOR, KNOWN_VERSION_MINOR);
            return false;
        }
        if (vPatch > KNOWN_VERSION_PATCH) {
            ENGINE_LOG_WARN("File is v{0}.{1}.{2}. Known version is v{3}.{4}.{5}. Should still be compatible.",
                vMajor, vMinor, vPatch,
                KNOWN_VERSION_MAJOR, KNOWN_VERSION_MINOR, KNOWN_VERSION_PATCH);
        }

        return true;
    };

    Mesh::~Mesh() {

    }

    Mesh::Mesh(const std::string& filename) {

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

        m_hasAnimations = Flag & 0x01;
        ENGINE_LOG_DEBUG(" Mesh has animations: {0}", m_hasAnimations);

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
        if (m_hasAnimations) {
            char BONE[4];
            file.read(BONE, 4);
            if (!checkTag(BONE, "BONE", 4)) return;

            u16 num_bones = read_u16(file);
            m_Skeleton.num_bones = num_bones;

            for (int n_bone = 0; n_bone < num_bones; n_bone++) {
                SkeleJoint joint;
                u16 name_len = read_u16(file);

                char* name = (char*)malloc(name_len);
                file.read(name, name_len);
                name[name_len - 1] = 0; // just in case...
                ENGINE_LOG_DEBUG("  Bone Name: '{0}'", name);
                joint.bone_name = std::string(name);
                free(name);

                joint.parent_idx = read_s32(file);
                joint.debug_length = read_f32(file);

                file.read(reinterpret_cast<char*>(&joint.local_matrix), 16 * sizeof(f32));
                file.read(reinterpret_cast<char*>(&joint.inverse_model_matrix), 16 * sizeof(f32));

                joint.model_matrix = laml::inverse(joint.inverse_model_matrix);
                joint.finalTransform = laml::Mat4(1.0f);

                m_Skeleton.bones.push_back(joint);
            }
        }

        // DATA block
        char DATA[4];
        file.read(DATA, 4);
        if (!checkTag(DATA, "DATA", 4)) return;

        // read indices
        std::vector<Triangle> m_Tris;
        m_Tris.resize(numInds * 3U);
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

        if (m_hasAnimations) {
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
                m_Vertices[n_vert].Texcoord = m_Vertices[n_vert].Texcoord * laml::Vec2(1.0f, -1.0f);
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

        // Animations catalog
        if (m_hasAnimations) {
            char ANIMS[4];
            file.read(ANIMS, 4);
            if (!checkTag(ANIMS, "ANIM", 4)) return;

            u16 num_anims = read_u16(file);
            for (int n_anim = 0; n_anim < num_anims; n_anim++) {
                u16 name_len = read_u16(file);

                std::string anim_name;
                anim_name.resize(name_len-1);
                file.read(anim_name.data(), name_len-1); // ignore null terminator
                char garb;
                file.read(&garb, 1);

                m_Animations[anim_name] = Animation();
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
        if (m_hasAnimations) {
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
        mat_spec.Albedo = MaterialCatalog::GetTexture("Data/Images/Stormtrooper_D.png");
        mat_spec.Normal = MaterialCatalog::GetTexture("Data/Images/normal.png");
        mat_spec.Ambient = MaterialCatalog::GetTexture("Data/Images/white.png");
        mat_spec.Metalness = MaterialCatalog::GetTexture("Data/Images/black.png");
        mat_spec.Roughness = MaterialCatalog::GetTexture("Data/Images/white.png");
        mat_spec.Emissive = MaterialCatalog::GetTexture("Data/Images/black.png");

        // mat_spec should now have all the valid data needed!
        // upload everyhing from mat_spec to the material
        m_BaseMaterial->Set<laml::Vec3>("u_AlbedoColor", mat_spec.AlbedoBase);
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

        //mat->Set<laml::Vec3>("u_AlbedoColor", laml::Vec3(1, .5, .5));
        //mat->Set<float>("u_Metalness", 1.0f);
        //mat->Set<float>("u_Roughness", 0.75f);
        //mat->Set<float>("u_TextureScale", 2.0f);
        m_Materials.push_back(mat);

        // Data is read from file, create gpu buffers now.
        {
            m_VertexArray = VertexArray::Create();

            auto vb = VertexBuffer::Create(vertex_data_ptr, vertex_data_size);
            free(vertex_data_ptr); vertex_data_ptr = nullptr;

            if (m_hasAnimations) {
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

        populateAnimationData(filename);

        // TMP
        //SetCurrentAnimation("Action");
        SetCurrentAnimation("dance");

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
        sm.Transform = laml::Mat4(1.0f);
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
            m_BaseMaterial->Set<laml::Vec3>("u_AlbedoColor", mat_spec.AlbedoBase);
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

            //mat->Set<laml::Vec3>("u_AlbedoColor", laml::Vec3(1, .5, .5));
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

    void Mesh::UpdateSkeleton(u32 f1, u32 f2, f32 interp) {
        for (u32 node_idx = 0; node_idx < m_currentAnim->num_nodes; node_idx++) {
            SkeleJoint& bone = m_Skeleton.bones[node_idx];
            const AnimNode& anim_node = m_currentAnim->nodes[node_idx];
        
            // interpolate between these two states
            laml::Vec3 position = laml::lerp(anim_node.translations[f1], anim_node.translations[f2], interp);
            laml::Quat rotation = laml::slerp(anim_node.rotations[f1], anim_node.rotations[f2], interp);
            laml::Vec3 scale    = laml::lerp(anim_node.scales[f1], anim_node.scales[f2], interp);
        
            // calc the local transform
            laml::Mat4 local_transform;
            laml::transform::create_transform(local_transform, rotation, position, scale);
        
            // get the global transform by mul with parent transform
            if (bone.parent_idx == Skeleton::NullIndex) {
                // no parent
                bone.finalTransform = local_transform;
            }
            else {
                const auto parent_idx = bone.parent_idx;
                ENGINE_LOG_ASSERT(node_idx > parent_idx, "Child bone referencing parent transform that hasn't been set yet!");
                auto parent_transform = m_Skeleton.bones[parent_idx].finalTransform;
                //bone.finalTransform = laml::mul(parent_transform, laml::mul(bone.local_matrix, local_transform));
                bone.finalTransform = laml::mul(parent_transform, local_transform);
            }
        }
    }

    void Mesh::OnUpdate(float dt) {
        // if has animations, update animation state.
        if (m_hasAnimations && m_currentAnim) {
            m_animTime += dt;
        
            if (m_currentAnim->anim_flag & 0x02) { // if looping enabled
                while (m_animTime > m_currentAnim->duration) {
                    m_animTime -= m_currentAnim->duration;
                    ENGINE_LOG_DEBUG("Looping animation!");
                }
            }
            else if (m_animTime > m_currentAnim->duration) {
                m_animTime = m_currentAnim->duration;
            }

            while (m_animTime < 0.0f) {
                m_animTime += m_currentAnim->duration;
            }
        
            float frame_num = m_animTime * m_currentAnim->frame_rate;
            u32 frame1 = floor(frame_num);
            u32 frame2 = ceil(frame_num);
            f32 interp = (frame_num - static_cast<f32>(frame1));
            if (frame2 == m_currentAnim->num_samples) {
                frame2 = 0;
                frame1 = m_currentAnim->num_samples - 1;
            }
        
            //ENGINE_LOG_DEBUG("Interpolating between frames {0} and {1} out of {2} [{3}]", frame1, frame2, m_currentAnim->num_samples, interp);
        
            UpdateSkeleton(frame1, frame2, interp);
        }
    }

    void Mesh::populateAnimationData(const std::string& filename) {
        // get base .mesh file name
        std::string path = filename.substr(0, filename.find_last_of("."));
        //size_t start = path.find_last_of("/") + 1;
        //std::string name = path.substr(start, path.size());
        //path = path.substr(0, start);

        for (auto &anim_entry : m_Animations) {
            auto anim_filename = path + std::string("_") + std::string(anim_entry.first) + std::string(".anim");
            auto& anim = anim_entry.second;

            // Open file at the end
            std::ifstream file{ anim_filename, std::ios::ate | std::ios::binary };

            if (!file.is_open()) {
                ENGINE_LOG_ERROR("Could not open file '{0}'!", anim_filename);
                return;
            }

            // read current offset (at end of file) giving filesize, then go back to the start
            size_t actual_fileSize = static_cast<size_t>(file.tellg());
            file.seekg(0);

            // HEADER
            char MAGIC[4];
            file.read(MAGIC, 4);
            if (!checkTag(MAGIC, "ANIM", 4)) return;

            u32 FileSize = read_u32(file);
            if (static_cast<size_t>(FileSize) != actual_fileSize) {
                ENGINE_LOG_ERROR("EROR::Expectied a fileSize of {0}, got {1}\n", actual_fileSize, FileSize);
                return;
            }

            char vStr[4];
            file.read(vStr, 4);
            if (!VERSION_CHECK(vStr)) return;

            anim.anim_flag = read_u32(file);

            //assert((Flag & 0x01) && (KNOWN_VERSION_MINOR == 9) && " ");

            anim.duration   = read_f32(file);
            anim.frame_rate = read_f32(file);
            anim.num_nodes  = read_u32(file);
            anim.num_samples = read_u32(file);

            char DATA[4];
            file.read(DATA, 4);
            if (!checkTag(DATA, "DATA", 4)) return;

            anim.nodes.resize(anim.num_nodes);
            for (u32 n = 0; n < anim.num_nodes; n++) {
                AnimNode& node = anim.nodes[n];
                node.node_flag   = read_u32(file);

                node.translations.resize(anim.num_samples);
                node.rotations.resize(anim.num_samples);
                node.scales.resize(anim.num_samples);

                for (u32 i = 0; i < anim.num_samples; i++) {
                    f32 x = read_f32(file);
                    f32 y = read_f32(file);
                    f32 z = read_f32(file);
                    node.translations[i] = laml::Vec3(x, y, z);
                }
                for (u32 i = 0; i < anim.num_samples; i++) {
                    //f32 x = read_f32(file);
                    //f32 y = read_f32(file);
                    //f32 z = read_f32(file);
                    //f32 w = read_f32(file);
                    //node.rotations[i] = laml::Quat(x, y, z, w);
                    
                    //node.rotations[i] = laml::Quat(read_f32(file), read_f32(file), read_f32(file), read_f32(file));
                    read_quat(file, &node.rotations[i]);
                }
                for (u32 i = 0; i < anim.num_samples; i++) {
                    f32 x = read_f32(file);
                    f32 y = read_f32(file);
                    f32 z = read_f32(file);
                    node.scales[i] = laml::Vec3(x, y, z);
                }
            }

            // Closing tag
            char END[4];
            file.read(END, 4);

            if (END[0] != 'E' || END[1] != 'N' || END[2] != 'D' || END[3] != '\0') {
                ENGINE_LOG_ERROR("ERROR::Did not reach the END tag at the end!!\n");
                return;
            }
        }
    }

    void Mesh::SetCurrentAnimation(const std::string& anim_name) {
        // TODO: VERY unsafe.
        // If m_Animations every gets added to, this reference becomes invalid.
        if (m_Animations.find(anim_name) != m_Animations.end()) {
            m_currentAnim = &m_Animations[anim_name];
            //m_animCache.resize(m_currentAnim->num_channels);

            ENGINE_LOG_INFO("Playing animation [{0}]", anim_name);
        }
        else {
            ENGINE_LOG_ERROR("Mesh does not have an animation called [{0}]", anim_name);
        }
    }
}
