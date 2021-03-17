#include <enpch.hpp>
#include "Material.hpp"

namespace Engine {

    Ref<Material> Material::Create(const Ref<Shader>& shader) {
        return std::make_shared<Material>(shader);
    }

    Material::Material(const Ref<Shader>& shader) 
        : m_Shader(shader) {

        AllocateStorage();
    }

    Material::~Material() {
    }

    void Material::AllocateStorage() {
        if (m_Shader->HasVertUniformBuffer()) {
            const auto& vertUniforms = m_Shader->GetVertUniformGroup();
            m_VertUniformStorage.Allocate(vertUniforms.GetSize());
            m_VertUniformStorage.ZeroInitialize();
        }

        if (m_Shader->HasFragUniformBuffer()) {
            const auto& fragUniforms = m_Shader->GetFragUniformGroup();
            m_FragUniformStorage.Allocate(fragUniforms.GetSize());
            m_FragUniformStorage.ZeroInitialize();
        }
    }

    ShaderUniformDeclaration * Material::FindUniformDeclaration(const std::string & name) {
        if (m_VertUniformStorage)
        {
            auto& declarations = m_Shader->GetVertUniformGroup().GetUniformDeclarations();
            for (ShaderUniformDeclaration* uniform : declarations)
            {
                if (uniform->GetName() == name)
                    return uniform;
            }
        }

        if (m_FragUniformStorage)
        {
            auto& declarations = m_Shader->GetFragUniformGroup().GetUniformDeclarations();
            for (ShaderUniformDeclaration* uniform : declarations)
            {
                if (uniform->GetName() == name)
                    return uniform;
            }
        }
        return nullptr;
    }

    ShaderSamplerDeclaration * Material::FindSamplerDeclaration(const std::string & name) {
        auto& samplers = m_Shader->GetSamplers();
        for (ShaderSamplerDeclaration* sampler : samplers)
        {
            if (sampler->GetName() == name)
                return sampler;
        }
        return nullptr;
    }

    Buffer& Material::GetUniformBufferTarget(ShaderUniformDeclaration* uniformDeclaration)
    {
        switch (uniformDeclaration->GetDomain())
        {
            case ShaderDomain::Vertex:    return m_VertUniformStorage;
            case ShaderDomain::Fragment:  return m_FragUniformStorage;
        }

        ENGINE_LOG_ASSERT(false, "Invalid uniform declaration domain! Material does not support this shader type.");
        return m_VertUniformStorage;
    }

    void Material::Bind()
    {
        m_Shader->Bind();

        if (m_VertUniformStorage)
            m_Shader->SetVertUniformBuffer(m_VertUniformStorage);

        if (m_FragUniformStorage)
            m_Shader->SetFragUniformBuffer(m_FragUniformStorage);

        BindTextures();
    }

    void Material::BindTextures() {
        for (size_t i = 0; i < m_Textures.size(); i++)
        {
            auto& texture = m_Textures[i];
            if (texture)
                texture->Bind(i);
        }
    }

    /* Material Instance ******************************************************/

    Ref<MaterialInstance> MaterialInstance::Create(const Ref<Material>& material, const std::string& name) {
        return std::make_shared<MaterialInstance>(material, name);
    }

    MaterialInstance::MaterialInstance(const Ref<Material>& material, const std::string & name) 
        : m_Material(material), m_Name(name) {

        m_Material->m_MaterialInstances.insert(this);

        AllocateStorage();
    }

    MaterialInstance::~MaterialInstance() {
        m_Material->m_MaterialInstances.erase(this);
    }

    void MaterialInstance::AllocateStorage() {
        if (m_Material->m_Shader->HasVertUniformBuffer()) {
            const auto& vertUniforms = m_Material->m_Shader->GetVertUniformGroup();
            m_VertStorage.Allocate(vertUniforms.GetSize());
            m_VertStorage.ZeroInitialize();
        }

        if (m_Material->m_Shader->HasFragUniformBuffer()) {
            const auto& fragUniforms = m_Material->m_Shader->GetFragUniformGroup();
            m_FragStorage.Allocate(fragUniforms.GetSize());
            m_FragStorage.ZeroInitialize();
        }
    }

    Buffer & MaterialInstance::GetUniformBufferTarget(ShaderUniformDeclaration * uniformDeclaration) {
        switch (uniformDeclaration->GetDomain())
        {
            case ShaderDomain::Vertex:      return m_VertStorage;
            case ShaderDomain::Fragment:    return m_FragStorage;
        }

        ENGINE_LOG_ASSERT(false, "Invalid uniform declaration domain! Material does not support this shader type.");
        return m_VertStorage;
    }

    void MaterialInstance::Bind() {

        // bind the base material
        m_Material->Bind();

        // if the material instance has anything to override, bind that
        m_Material->m_Shader->Bind();

        if (m_VertStorage)
            m_Material->m_Shader->SetVertUniformBuffer(m_VertStorage, m_OverriddenValues);

        if (m_FragStorage)
            m_Material->m_Shader->SetFragUniformBuffer(m_FragStorage, m_OverriddenValues);

        //m_Material->BindTextures();
        for (size_t i = 0; i < m_Textures.size(); i++)
        {
            auto& texture = m_Textures[i];
            if (texture)
                texture->Bind(i);
        }
    }
}