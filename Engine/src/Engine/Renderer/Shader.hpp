#pragma once

#include "Engine/Buffer.hpp"
#include "Engine/Core/GameMath.hpp"

namespace rh {

    enum class ShaderDomain {
        None = 0,
        Vertex = 1,
        Fragment = 2
    };

    class ShaderUniformDeclaration {
    private:
        friend class Shader;
        friend class OpenGlShader;
        friend class ShaderStruct;
    public:
        virtual const std::string& GetName() const = 0;
        virtual u32 GetSize() const = 0;
        virtual u32 GetCount() const = 0;
        virtual u32 GetOffset() const = 0;
        virtual ShaderDomain GetDomain() const = 0;

    protected:
        virtual void SetOffset(u32 offset) = 0;
    };

    class ShaderSamplerDeclaration {
    public:
        virtual const std::string& GetName() const = 0;
        virtual u32 GetCount() const = 0;
        virtual u32 GetID() const = 0;
    };
    
    class ShaderUniformGroupDeclaration {
    public:
        virtual const std::string& GetName() const = 0;
        virtual u32 GetSize() const = 0;
        virtual const std::vector<ShaderUniformDeclaration*>& GetUniformDeclarations() const = 0;
        virtual ShaderUniformDeclaration* FindUniformDeclaration(const std::string& name) const = 0;
    };

    class ShaderStruct {
    private:
        friend class Shader;

    public:
        ShaderStruct(const std::string& name) :m_Name(name), m_Size(0), m_Offset(0) {}

        void AddField(ShaderUniformDeclaration* field);

        inline void SetOffset(u32 offset) { m_Offset = offset; }

        inline const std::string& GetName() const { return m_Name; }
        inline u32 GetSize() const { return m_Size; }
        inline u32 GetOffset() const { return m_Offset; }
        inline const std::vector<ShaderUniformDeclaration*>& GetFields() const { return m_Fields; }

        void Cleanup();

    private:
        std::string m_Name;
        std::vector<ShaderUniformDeclaration*> m_Fields;
        u32 m_Size;
        u32 m_Offset;
    };

    class Shader {
    public:
        virtual ~Shader() = default;

        virtual void Bind() const = 0;
        virtual void UnBind() const = 0;
        virtual void Reload() = 0;

        virtual const std::string& GetName() const = 0;

        virtual void SetVertUniformBuffer(Buffer buffer) = 0;
        virtual void SetFragUniformBuffer(Buffer buffer) = 0;
        virtual void SetVertUniformBuffer(Buffer buffer, const std::unordered_set<std::string>& overrides) = 0;
        virtual void SetFragUniformBuffer(Buffer buffer, const std::unordered_set<std::string>& overrides) = 0;

        virtual bool HasFragUniformBuffer() const = 0;
        virtual bool HasVertUniformBuffer() const = 0;
        virtual const ShaderUniformGroupDeclaration& GetVertUniformGroup() const = 0;
        virtual const ShaderUniformGroupDeclaration& GetFragUniformGroup() const = 0;
        virtual const std::vector<ShaderSamplerDeclaration*>& GetSamplers() const = 0;

        virtual void SetMat4(const std::string& name, const math::mat4& value) const = 0;
        virtual void SetFloat(const std::string &name, f32 value) const = 0;
        virtual void SetInt(const std::string &name, s32 value) const = 0;
        virtual void SetVec2(const std::string &name, const math::vec2& value) const = 0;
        virtual void SetVec3(const std::string &name, const math::vec3& value) const = 0;
        virtual void SetVec4(const std::string &name, const math::vec4& value) const = 0;

        static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
        static Ref<Shader> Create(const std::string& path);
    };

    class ShaderLibrary {
    public:
        void Add(const Ref<Shader>& shader);
        void Add(const Ref<Shader>& shader, const std::string& name);

        Ref<Shader> Load(const std::string& path);
        Ref<Shader> Load(const std::string& path, const std::string& name);

        Ref<Shader> Get(const std::string& name);

        bool Exists(const std::string& name) const;

        void ReloadAll();

    private:
        std::unordered_map<std::string, Ref<Shader>> m_Shaders;
    };
}