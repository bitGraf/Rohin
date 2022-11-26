#pragma once

#include "Engine/Core/Base.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Resources/nbt/nbt.hpp"

namespace rh {

    struct MaterialSpec {
        std::string Name;
        Texture2D* Albedo = nullptr;
        Texture2D* Normal = nullptr;
        Texture2D* Ambient = nullptr;
        Texture2D* Metalness = nullptr;
        Texture2D* Roughness = nullptr;
        Texture2D* Emissive = nullptr;
        laml::Vec3 AlbedoBase;
        float RoughnessBase = 1;
        float MetalnessBase = 0;
        float TextureScale = 1;
    };

    namespace MaterialCatalog {
        void Create();
        void Destroy();

        MaterialSpec GetMaterial(const std::string& material_name);

        Texture2D* GetTexture(const std::string& texture_path);
        Texture2D* GetTexture(const unsigned char* bitmap, u32 res);
        TextureCube* GetTextureCube(const std::string& texture_path);

        void RegisterMaterial(const std::string& mat_name, const nbt::tag_compound& data);
        // ANIM_HOOK void RegisterMaterial(const std::unordered_map<std::string, md5::Material>& materialMap);
    };
}