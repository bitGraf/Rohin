#include <enpch.hpp>
#include "MeshCatalog.hpp"

namespace rh {

    namespace MeshCatalog {
        std::unordered_map<std::string, Mesh*> m_MeshList;

        void Register(const std::string& mesh_name, const std::string& filepath, FileFormat file_type) {
            switch (file_type) {
            case FileFormat::NBT_Basic: {
                Mesh* mesh = new Mesh(filepath, 0.0f, 0.0f);
                if (mesh->Loaded()) {
                    m_MeshList[mesh_name] = mesh;
                    ENGINE_LOG_INFO("Registering Mesh: [{0}] from [{1}]", mesh_name, filepath);
                }
            }break;
            case FileFormat::MESH_File: {
                Mesh* mesh = new Mesh(filepath);
                if (mesh->Loaded()) {
                    m_MeshList[mesh_name] = mesh;
                    ENGINE_LOG_INFO("Registering Mesh: [{0}] from [{1}]", mesh_name, filepath);
                }
                else {
                    ENGINE_LOG_ERROR("Failed to load Mesh [{0}] from [{1}]", mesh_name, filepath);
                }
            }break;
            case FileFormat::None: {
                ENGINE_LOG_ERROR("Proper Mesh FileFormat not specified!");
            }break;
            }
        }

        Mesh* Get(const std::string& mesh_name) {
            if (m_MeshList.find(mesh_name) == m_MeshList.end()) {
                return nullptr;
            }

            return m_MeshList[mesh_name];
        }

        void Create() {

        }
        
        void Destroy() {
            // deallocate memory
            for (const auto& it : m_MeshList) {
                delete it.second;
            }
            // Clear the mesh catalog of all entries
            m_MeshList.clear();
        }
    }
}