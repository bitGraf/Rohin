#pragma once

#include "Engine/Core/Base.hpp"
#include "Engine/Resources/MeshCatalog.hpp"

namespace Engine {

    namespace AnimCatalog {
        // Register a mesh from file
        void Register(const std::string& anim_name, const std::string& filepath, FileFormat file_type);
        md5::Animation* Get(const std::string& anim_name);

        void Create();
        void Destroy();
    };
}