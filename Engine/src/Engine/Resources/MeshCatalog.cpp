#include <enpch.hpp>
#include "MeshCatalog.hpp"

namespace Engine {

    static std::unordered_map<std::string, std::shared_ptr<Mesh>> m_MeshList;

    void MeshCatalog::Register(const std::string& mesh_name, const std::string& mesh_path, bool nbt) {
        ENGINE_LOG_INFO("Registering Mesh: [{0}] from [{1}]", mesh_name, mesh_path);
        auto mesh = std::make_shared<Engine::Mesh>(mesh_path, nbt);
        m_MeshList[mesh_name] = mesh;
    }

    std::shared_ptr<Mesh> MeshCatalog::Get(const std::string& mesh_name) {
        // TODO: dangerous, need to check if mesh_name exists in the map
        return m_MeshList[mesh_name];
    }

    void MeshCatalog::Clear() {
        // Clear the mesh catalog of all entries
        m_MeshList.clear();
    }

}