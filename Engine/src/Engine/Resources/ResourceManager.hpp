#pragma once

#include "Engine/Core/Base.hpp"
#include "Engine/Resources/Catalog.hpp"
#include "Engine/Renderer/Texture.hpp"

namespace rh {

    struct Texture_Tag {
        Texture2D* texture;
    };

    namespace ResourceManager {
        Texture_Tag GetTexture(const std::string& tex_name);
        void RegisterTexture(const std::string& path);

        void CreateBuffers();
        void ResetBuffers();
        void DestroyBuffers();
    };
}