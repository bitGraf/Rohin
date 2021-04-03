#pragma once

#include "Engine/Core/Base.hpp"
#include "Engine/Renderer/Mesh.hpp"

#include "Engine/Resources/MD5MeshLoader.hpp"

namespace Engine {

    class MeshCatalog {
    public:
        static void Register(const std::string& mesh_name, const md5::Model& model);
        static void Register(const std::string& mesh_name, const std::string& mesh_path, bool nbt);
        static Ref<Mesh> Get(const std::string& mesh_name);
        static void Clear();

    private:
        MeshCatalog() {}
    };
}