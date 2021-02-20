#include <enpch.hpp>
#include "MeshCatalog.hpp"

namespace Engine {

    std::unordered_map<std::string, std::shared_ptr<Mesh>> m_MeshList;

    void MeshCatalog::Register(const std::string& mesh_name, const std::string& mesh_path) {
        auto mesh = std::make_shared<Engine::Mesh>(mesh_path);
        m_MeshList[mesh_name] = mesh;
    }

    std::shared_ptr<Mesh> MeshCatalog::Get(const std::string& mesh_name) {
        // TODO: dangerous, need to check if mesh_name exists in the map
        return m_MeshList[mesh_name];
    }

}