#pragma once

#include "Engine/Renderer/Shader.hpp"

typedef unsigned int GLenum;

namespace rh {

    class OpenGLShaderUniformDeclaration : public ShaderUniformDeclaration {
    private:
        friend class OpenGLShader;
        friend class OpenGLShaderUniformGroupDeclaration;
    public:
        enum class Type
        {
            NONE, FLOAT32, VEC2, VEC3, VEC4, MAT3, MAT4, INT32, STRUCT
        };
    public:
        OpenGLShaderUniformDeclaration(ShaderDomain domain, Type type, const std::string& name, u32 count);
        OpenGLShaderUniformDeclaration(ShaderDomain domain, ShaderStruct* sStruct, const std::string& name, u32 count);

        virtual const std::string& GetName() const override { return m_Name; }
        virtual u32 GetSize() const override { return m_Size; }
        virtual u32 GetCount() const override { return m_Count; }
        virtual u32 GetOffset() const override { return m_Offset; }
        virtual ShaderDomain GetDomain() const override { return m_Domain; }

        inline Type GetType() const { return m_Type; }
        inline u32 GetLocation() const { return m_Location; }

    private:
        virtual void SetOffset(u32 offset) override;
        void SetLocation(u32 location) { m_Location = location; }

        static Type StringToType(const std::string& typeString);
        static u32 SizeOfUniformType(Type type);

    private:
        std::string m_Name;
        u32 m_Size, m_Offset;
        ShaderDomain m_Domain; // shader source type
        Type m_Type; // if a simple variable
        ShaderStruct* m_Struct; // if a struct
        u32 m_Count; // if an array of vars
        u32 m_Location; // OpenGL location id
    };

    class OpenGLShaderSamplerDeclaration : public ShaderSamplerDeclaration
    {
    public:
        enum class Type
        {
            NONE, TEXTURE2D, TEXTURECUBE
        };
    private:
        friend class OpenGLShader;
    public:
        OpenGLShaderSamplerDeclaration(Type type, const std::string& name, uint32_t count);

        inline const std::string& GetName() const override { return m_Name; }
        inline uint32_t GetID() const override { return m_ID; }
        inline uint32_t GetCount() const override { return m_Count; }

        inline Type GetType() const { return m_Type; }

    public:
        static Type StringToType(const std::string& type);
        static std::string TypeToString(Type type);

    private:
        std::string m_Name;
        uint32_t m_ID = 0;
        uint32_t m_Count;
        Type m_Type;
    };

    class OpenGLShaderUniformGroupDeclaration : public ShaderUniformGroupDeclaration {
    private:
        friend class Shader;
    public:
        //"vertShaderGroup", domain
        OpenGLShaderUniformGroupDeclaration(const std::string& name, ShaderDomain domain);
        virtual ~OpenGLShaderUniformGroupDeclaration();

        virtual const std::string& GetName() const override { return m_Name; }
        virtual u32 GetSize() const override { return m_Size; }
        virtual const std::vector<ShaderUniformDeclaration*>& GetUniformDeclarations() const override { return m_Uniforms; }
        virtual ShaderUniformDeclaration* FindUniformDeclaration(const std::string& name) const override;

        void AddUniform(OpenGLShaderUniformDeclaration* uniform);
        ShaderDomain GetDomain() const { return m_Domain; }

        void Reset();

    private:
        std::string m_Name;
        u32 m_Size;
        std::vector<ShaderUniformDeclaration*> m_Uniforms;
        ShaderDomain m_Domain;
    };

    class OpenGLShader : public Shader {
    public:
        OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
        OpenGLShader(const std::string& path);
        virtual ~OpenGLShader();

        virtual void Bind() const override;
        virtual void UnBind() const override;
        virtual void Reload() override;
        void CompileAndValidate();

        virtual const std::string& GetName() const override { return m_Name; }

        // Set uniforms
        /*
        void SetBool(const std::string &name, bool value) const;
        void SetInt(const std::string &name, s32 value) const;

        void SetVec2(const std::string &name, const math::vec2& value) const;
        void SetVec4(const std::string &name, const math::vec4& value) const;

        void SetMat2(const std::string &name, const math::mat2& value) const;
        void SetMat3(const std::string &name, const math::mat3& value) const;
        */
        virtual void SetFloat(const std::string &name, f32 value) const override;
        virtual void SetInt(const std::string &name, s32 value) const override;
        virtual void SetVec2(const std::string &name, const laml::Vec2& value) const override;
        virtual void SetVec3(const std::string &name, const laml::Vec3& value) const override;
        virtual void SetVec4(const std::string &name, const laml::Vec4& value) const override;
        virtual void SetMat4(const std::string &name, const laml::Mat4& value) const override;

        // Upload data to uniform location
        void UploadUniformInt(uint32_t location, int32_t value);
        void UploadUniformIntArray(uint32_t location, int32_t* values, int32_t count);
        void UploadUniformFloat(uint32_t location, float value);
        void UploadUniformFloat2(uint32_t location, const laml::Vec2& value);
        void UploadUniformFloat3(uint32_t location, const laml::Vec3& value);
        void UploadUniformFloat4(uint32_t location, const laml::Vec4& value);
        void UploadUniformMat3(uint32_t location, const laml::Mat3& values);
        void UploadUniformMat4(uint32_t location, const laml::Mat4& values);
        void UploadUniformMat4Array(uint32_t location, const laml::Mat4& values, uint32_t count);

        void UploadUniformStruct(OpenGLShaderUniformDeclaration* uniform, byte* buffer, uint32_t offset);

        /* Access uniform declarations */
        virtual void SetVertUniformBuffer(Buffer buffer) override;
        virtual void SetFragUniformBuffer(Buffer buffer) override;
        virtual void SetVertUniformBuffer(Buffer buffer, const std::unordered_set<std::string>& overrides) override;
        virtual void SetFragUniformBuffer(Buffer buffer, const std::unordered_set<std::string>& overrides) override;

        virtual bool HasFragUniformBuffer() const override { return (bool)m_FragUniformGroup; }
        virtual bool HasVertUniformBuffer() const override { return (bool)m_VertexUniformGroup; }
        virtual const ShaderUniformGroupDeclaration& GetVertUniformGroup() const override { return *m_VertexUniformGroup; }
        virtual const ShaderUniformGroupDeclaration& GetFragUniformGroup() const override { return *m_FragUniformGroup; }
        virtual const std::vector<ShaderSamplerDeclaration*>& GetSamplers() const override { return m_Samplers; }


    private:
        std::string ReadFile(const std::string& path);
        std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
        s32 GetUniformLocation(const std::string& name) const;

        void ResolveAndSetUniforms(const Ref<OpenGLShaderUniformGroupDeclaration>& decl, Buffer buffer);
        void ResolveAndSetUniforms(const Ref<OpenGLShaderUniformGroupDeclaration>& decl, Buffer buffer, const std::unordered_set<std::string>& overrides);
        void ResolveAndSetUniform(OpenGLShaderUniformDeclaration* uniform, Buffer buffer);
        void ResolveAndSetUniformArray(OpenGLShaderUniformDeclaration* uniform, Buffer buffer);
        void ResolveAndSetUniformField(const OpenGLShaderUniformDeclaration& field, byte* data, int32_t offset);

    private:
        // Uniform logic
        void ParseUniform(const std::string& statement, ShaderDomain domain);
        void ParseUniformStruct(const std::string& statement, ShaderDomain domain);
        void ValidateUniforms();

        void Compile();
        void IdentifyUniforms();
        ShaderStruct* FindStruct(const std::string& structName) const;

    private:
        std::unordered_map<GLenum, std::string> m_ShaderSources;
        Ref<OpenGLShaderUniformGroupDeclaration> m_FragUniformGroup;
        Ref<OpenGLShaderUniformGroupDeclaration> m_VertexUniformGroup;
        std::vector<ShaderStruct*> m_Structs;
        std::vector<ShaderSamplerDeclaration*> m_Samplers;

        std::string m_Name;
        std::string m_filepath;
        u32 m_ShaderID;
        bool m_Loaded;
    };
}