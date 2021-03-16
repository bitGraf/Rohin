#pragma once

#include "Engine/Core/Base.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Resources/nbt/nbt.hpp"

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
        float RoughnessBase;
        float MetalnessBase;
    };

    class MaterialCatalog {
    public:
        static void Init();
        static void Shutdown();

        static MaterialSpec GetMaterial(const std::string& material_name);
        static Ref<Texture2D> GetTexture(const std::string& texture_path);

        static void RegisterMaterial(const std::string& mat_name, const nbt::tag_compound& data);

    private:
        MaterialCatalog() {}
    };
}