#pragma once

#include "Engine/Core/Base.hpp"
#include "Engine/Renderer/Mesh.hpp"

#include "Engine/Resources/MD5MeshLoader.hpp"

namespace Engine {

    namespace MeshCatalog {
        void Register(const std::string& mesh_name, const md5::Model& model);
        void Register(const std::string& mesh_name, const std::string& mesh_path);
        Ref<Mesh> Get(const std::string& mesh_name);
        void Clear();
    };
}