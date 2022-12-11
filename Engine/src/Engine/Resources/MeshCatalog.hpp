#pragma once

#include "Engine/Core/Base.hpp"
#include "Engine/Renderer/Mesh.hpp"

namespace rh {

    enum class FileFormat : unsigned char {
        None = 0,
        NBT_Basic,
        MESH_File
    };

    namespace MeshCatalog {
        // Register a mesh from file
        void Register(const std::string& mesh_name, const std::string& filepath, FileFormat file_type);
        Mesh* Get(const std::string& mesh_name);
        
        void Create();
        void Destroy();
    };
}