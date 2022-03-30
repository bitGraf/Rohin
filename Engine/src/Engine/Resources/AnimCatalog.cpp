#include <enpch.hpp>
#include "AnimCatalog.hpp"

namespace Engine {

    namespace AnimCatalog {
        std::unordered_map<std::string, md5::Animation*> m_AnimList;

        void Register(const std::string& anim_name, const std::string& filepath, FileFormat file_type) {
            switch (file_type) {
            case FileFormat::MD5_Text: {
                md5::Animation* anim = new md5::Animation();
                md5::LoadMD5AnimFile(filepath, anim);

                if (true /*anim->Loaded()*/) { // TODO: check if animation was loaded correctly
                    m_AnimList[anim_name] = anim;
                    ENGINE_LOG_INFO("Registering Animation: [{0}] from [{1}]", anim_name, filepath);
                }
            }break;
            case FileFormat::NBT_Basic: {
                ENGINE_LOG_ERROR("Proper Animation FileFormat not specified!");
                /*
                Mesh* mesh = new Mesh(filepath);
                if (mesh->Loaded()) {
                    m_MeshList[anim_name] = mesh;
                    ENGINE_LOG_INFO("Registering Animation: [{0}] from [{1}]", anim_name, filepath);
                }
                */
            }break;
            case FileFormat::None: {
                ENGINE_LOG_ERROR("Proper Animation FileFormat not specified!");
            }break;
            }
        }

        md5::Animation* Get(const std::string& anim_name) {
            if (m_AnimList.find(anim_name) == m_AnimList.end()) {
                return nullptr;
            }

            return m_AnimList[anim_name];
        }

        void Create() {

        }

        void Destroy() {
            // deallocate memory
            for (const auto& it : m_AnimList) {
                delete it.second;
            }
            // Clear the mesh catalog of all entries
            m_AnimList.clear();
        }
    }
}