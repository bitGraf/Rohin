#pragma once

#include "Engine/Core/Base.hpp"
#include "Engine/Renderer/Mesh.hpp"

#include "Engine/Resources/MD5MeshLoader.hpp"

namespace Engine {

    enum class FileFormat : unsigned char {
        None = 0,
        NBT_Basic,
        MD5_Text
    };

    namespace MeshCatalog {
        // Register a mesh from file
        void Register(const std::string& mesh_name, const std::string& filepath, FileFormat file_type);
        Mesh* Get(const std::string& mesh_name);
        
        void Create();
        void Destroy();
    };
}