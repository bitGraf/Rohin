#pragma once

#include "Engine/Core/Base.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Resources/nbt/nbt.hpp"
#include "Engine/Resources/MD5MeshLoader.hpp"

namespace Engine {

    struct MaterialSpec {
        std::string Name;
        Ref<Texture2D> Albedo;
        Ref<Texture2D> Normal;
        Ref<Texture2D> Ambient;
        Ref<Texture2D> Metalness;
        Ref<Texture2D> Roughness;
        Ref<Texture2D> Emissive;
        math::vec3 AlbedoBase;
        float RoughnessBase = 1;
        float MetalnessBase = 0;
        float TextureScale = 1;
    };

    namespace MaterialCatalog {
        void Init();
        void Shutdown();

        MaterialSpec GetMaterial(const std::string& material_name);
        Ref<Texture2D> GetTexture(const std::string& texture_path);

        void RegisterMaterial(const std::string& mat_name, const nbt::tag_compound& data);
        void RegisterMaterial(const std::unordered_map<std::string, md5::Material>& materialMap);
    };
}