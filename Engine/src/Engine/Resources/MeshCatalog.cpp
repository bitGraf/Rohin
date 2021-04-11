#include <enpch.hpp>
#include "MeshCatalog.hpp"

namespace Engine {

    namespace MeshCatalog {

        std::unordered_map<std::string, Ref<Mesh>> m_MeshList;

        void Register(const std::string& mesh_name, const std::string& mesh_path) {
            ENGINE_LOG_INFO("Registering Mesh: [{0}] from [{1}]", mesh_name, mesh_path);
            auto mesh = std::make_shared<Engine::Mesh>(mesh_path);
            if (mesh->Loaded()) {
                m_MeshList[mesh_name] = mesh;
            }
        }

        void Register(const std::string& mesh_name, const md5::Model& model) {
            ENGINE_LOG_INFO("Registering Mesh: [{0}] from MD5Model", mesh_name);
            auto mesh = std::make_shared<Engine::Mesh>(model);
            if (mesh->Loaded()) {
                m_MeshList[mesh_name] = mesh;
            }
        }

        Ref<Mesh> Get(const std::string& mesh_name) {
            if (m_MeshList.find(mesh_name) == m_MeshList.end()) {
                return nullptr;
            }

            return m_MeshList[mesh_name];
        }

        void Clear() {
            // Clear the mesh catalog of all entries
            m_MeshList.clear();
        }
    }
}