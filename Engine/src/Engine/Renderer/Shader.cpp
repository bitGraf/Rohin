#include <enpch.hpp>
#include "Shader.hpp"
#include "Renderer.hpp"

#include "Engine/Platform/OpenGL/OpenGLShader.hpp"

namespace rh {

    Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) {
        switch (Renderer::GetAPI()) {
            case RendererAPI::API::None:
                ENGINE_LOG_ASSERT(false, "No API selected when creating vertexArray");
                return nullptr;
                break;
            case RendererAPI::API::OpenGL:
                return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
                break;
        }

        ENGINE_LOG_ASSERT(false, "Unknown rendererAPI selected");
        return nullptr;
    }

    Ref<Shader> Shader::Create(const std::string& path) {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            ENGINE_LOG_ASSERT(false, "No API selected when creating vertexArray");
            return nullptr;
            break;
        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLShader>(path);
            break;
        }

        ENGINE_LOG_ASSERT(false, "Unknown rendererAPI selected");
        return nullptr;
    }


    /* SHADER LIBRARY */
    void ShaderLibrary::Add(const Ref<Shader>& shader) {
        auto& name = shader->GetName();
        Add(shader, name);
    }

    void ShaderLibrary::Add(const Ref<Shader>& shader, const std::string& name) {
        ENGINE_LOG_ASSERT(!Exists(name), "Shader already exists");
        m_Shaders[name] = shader;
    }

    Ref<Shader> ShaderLibrary::Load(const std::string& path) {
        auto shader = Shader::Create(path);
        Add(shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::Load(const std::string& path, const std::string& name) {
        auto shader = Shader::Create(path);
        Add(shader, name);
        return shader;
    }

    Ref<Shader> ShaderLibrary::Get(const std::string& name) {
        ENGINE_LOG_ASSERT(Exists(name), "Shader not found");
        return m_Shaders[name];
    }

    bool ShaderLibrary::Exists(const std::string& name) const {
        return m_Shaders.find(name) != m_Shaders.end();
    }

    void ShaderLibrary::ReloadAll() {
        for (auto& shader : m_Shaders) {
            shader.second->Reload();
            ENGINE_LOG_INFO("Shader {0} recompiled.", shader.first);
        }
    }

    /* Shader Struct */
    void ShaderStruct::AddField(ShaderUniformDeclaration* field) {
        m_Size += field->GetSize();
        u32 offset = 0;
        if (m_Fields.size()) {
            ShaderUniformDeclaration* previous = m_Fields.back();
            offset = previous->GetOffset() + previous->GetSize();
        }
        field->SetOffset(offset);
        m_Fields.push_back(field);
    }

    void ShaderStruct::Cleanup() {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            ENGINE_LOG_ASSERT(false, "No API selected when creating vertexArray");
            return;
        case RendererAPI::API::OpenGL:
            for (auto field : m_Fields) {
                auto fieldCast = static_cast<OpenGLShaderUniformDeclaration*>(field);
                delete fieldCast;
                fieldCast = nullptr;
            }
            return;
        }

        ENGINE_LOG_ASSERT(false, "Unknown rendererAPI selected");
    }
}