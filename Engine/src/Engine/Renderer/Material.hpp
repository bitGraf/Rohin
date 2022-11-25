#pragma once

#include "Engine/Core/Base.hpp"

#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Buffer.hpp"

#include <unordered_set>

namespace rh {

    class Material {
        friend class MaterialInstance;
    public:
        Material(const Ref<Shader>& shader);
        virtual ~Material();

        void Bind();

        template <typename T>
        void Set(const std::string& name, const T& value)
        {
            auto decl = FindUniformDeclaration(name);
            ENGINE_LOG_ASSERT(decl, "Could not find uniform with name 'x'");
            auto& buffer = GetUniformBufferTarget(decl);
            buffer.Write((byte*)&value, decl->GetSize(), decl->GetOffset());
        }

        void Set(const std::string& name, Texture* texture)
        {
            auto decl = FindSamplerDeclaration(name);
            uint32_t slot = decl->GetID();
            if (m_Textures.size() <= slot)
                m_Textures.resize((size_t)slot + 1);
            m_Textures[slot] = texture;
        }

        void Set(const std::string& name, Texture2D* texture) {
            Set(name, (Texture*)texture);
        }
        void Set(const std::string& name, TextureCube* texture) {
            Set(name, (Texture*)texture);
        }

        template<typename T>
        T& Get(const std::string& name)
        {
            auto decl = FindUniformDeclaration(name);
            ENGINE_LOG_ASSERT(decl, "Could not find uniform with name 'x'");
            auto& buffer = GetUniformBufferTarget(decl);
            return buffer.Read<T>(decl->GetOffset());
        }

        template<typename T>
        Ref<T> GetResource(const std::string& name)
        {
            auto decl = FindSamplerDeclaration(name);
            uint32_t slot = decl->GetID();
            ENGINE_LOG_ASSERT(slot < m_Textures.size(), "Texture slot is invalid!");
            return m_Textures[slot];
        }

    public:
        static Ref<Material> Create(const Ref<Shader>& shader);

    private:
        void AllocateStorage();
        void BindTextures();

        ShaderUniformDeclaration * FindUniformDeclaration(const std::string& name);
        ShaderSamplerDeclaration* FindSamplerDeclaration(const std::string& name);
        Buffer& GetUniformBufferTarget(ShaderUniformDeclaration* uniformDeclaration);

    private:
        Ref<Shader> m_Shader;
        std::unordered_set<MaterialInstance*> m_MaterialInstances;

        Buffer m_VertUniformStorage;
        Buffer m_FragUniformStorage;
        std::vector<Texture*> m_Textures;
    };

    class MaterialInstance {
        friend class Material;
    public:
        MaterialInstance(const Ref<Material>& material, const std::string& name);
        virtual ~MaterialInstance();

        void Bind();
        Ref<Shader> GetShader() { return m_Material->m_Shader; }
        const std::string& GetName() { return m_Name; }

        template<typename T>
        void Set(const std::string& name, const T& value) {
            auto decl = m_Material->FindUniformDeclaration(name);
            if (!decl)
                return;
            ENGINE_LOG_ASSERT(decl, "Could not find uniform");
            auto& buffer = GetUniformBufferTarget(decl);
            buffer.Write((byte*)&value, decl->GetSize(), decl->GetOffset());

            m_OverriddenValues.insert(name);
        }

        void Set(const std::string& name, Texture* texture) {
            auto decl = m_Material->FindSamplerDeclaration(name);
            if (!decl)
            {
                ENGINE_LOG_WARN("Cannot find material property: {0}", name);
                return;
            }
            u32 slot = decl->GetID();
            if (m_Textures.size() <= slot)
                m_Textures.resize((size_t)slot + 1);
            m_Textures[slot] = texture;
        }

        void Set(const std::string& name, Texture2D* texture) {
            Set(name, (Texture*)texture);
        }
        void Set(const std::string& name, TextureCube* texture) {
            Set(name, (Texture*)texture);
        }

        template<typename T>
        T& Get(const std::string& name) {
            auto decl = m_Material->FindUniformDeclaration(name);
            ENGINE_LOG_ASSERT(decl, "Could not find uniform");
            auto& buffer = GetUniformBufferTarget(decl);
            return buffer.Read<T>(decl->GetOffset());
        }

        template<typename T>
        Ref<T> GetTexture(const std::string& name)
        {
            auto decl = m_Material->FindResourceDeclaration(name);
            ENGINE_LOG_ASSERT(decl, "Could not find uniform");
            uint32_t slot = decl->GetRegister();
            ENGINE_LOG_ASSERT(slot < m_Textures.size(), "Texture slot is invalid!");
            return Ref<T>(m_Textures[slot]);
        }

    public:
        static Ref<MaterialInstance> Create(const Ref<Material>& material, const std::string& name = std::string());

    private:
        void AllocateStorage();

        Buffer & GetUniformBufferTarget(ShaderUniformDeclaration* uniformDeclaration);

    private:
        Ref<Material> m_Material;
        std::string m_Name;

        Buffer m_VertStorage;
        Buffer m_FragStorage;
        std::vector<Texture*> m_Textures;

        std::unordered_set<std::string> m_OverriddenValues;
    };
}