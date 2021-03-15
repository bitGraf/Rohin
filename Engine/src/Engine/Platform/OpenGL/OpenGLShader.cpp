#include <enpch.hpp>
#include "OpenGLShader.hpp"

#include <fstream>
#include <glad/glad.h>

namespace Engine {

    static GLenum ShaderTypeFromString(const std::string& type) {
        if (type == "vertex")
            return GL_VERTEX_SHADER;
        if (type == "fragment" || type == "pixel")
            return GL_FRAGMENT_SHADER;
        if (type == "geometry")
            return GL_GEOMETRY_SHADER;

        ENGINE_LOG_ASSERT(false, "Unknown shader type");
        return 0;
    }

    OpenGLShader::OpenGLShader(const std::string& path) : m_Loaded(false) {
        std::string source = ReadFile(path);
        m_ShaderSources = PreProcess(source);

        // run_tree/Data/Shaders/simple.glsl
        auto lastSlash = path.find_last_of("/\\");
        lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
        auto lastDot = path.rfind('.');
        auto count = lastDot == std::string::npos ? path.size() - lastSlash : lastDot - lastSlash;

        m_Name = path.substr(lastSlash, count);

        Reload();
    }

    OpenGLShader::OpenGLShader(const std::string& name,  const std::string& vertexSrc, const std::string& fragmentSrc) 
            : m_Name(name) {
        m_ShaderSources[GL_VERTEX_SHADER] = vertexSrc;
        m_ShaderSources[GL_FRAGMENT_SHADER] = fragmentSrc;

        Reload();
    }

    void OpenGLShader::Reload() {
        // cpu side
        IdentifyUniforms();

        // gpu side
        Compile();

        ValidateUniforms();

        if (m_Loaded) {
            ENGINE_LOG_WARN("Shader has been reloaded! need to implement reloading callbacks still");
        }

        m_Loaded = true;
    }

    OpenGLShader::~OpenGLShader() {
        glDeleteProgram(m_ShaderID);

        for (auto sampler : m_Samplers) {
            auto samplerCast = static_cast<OpenGLShaderSamplerDeclaration*>(sampler);

            delete samplerCast;
            samplerCast = 0;
        }
    }

    void OpenGLShader::ValidateUniforms() {
        glUseProgram(m_ShaderID);

        // validate Vertex Shader Uniforms
        {
            const auto& decl = m_VertexUniformGroup;
            if (decl) {
                const std::vector<ShaderUniformDeclaration*>& uniformList = decl->GetUniformDeclarations();
                for (int n = 0; n < uniformList.size(); n++) {
                    OpenGLShaderUniformDeclaration* uniform = (OpenGLShaderUniformDeclaration*)uniformList[n];

                    if (uniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT) {
                        // parse the struct fields
                        const ShaderStruct& s = *uniform->m_Struct;
                        const auto& fields = s.GetFields();
                        for (int k = 0; k < fields.size(); k++) {
                            OpenGLShaderUniformDeclaration* field = (OpenGLShaderUniformDeclaration*)fields[k];
                            field->SetLocation(GetUniformLocation(uniform->m_Name + "." + field->m_Name));
                        }
                    }
                    else {
                        if (uniform->m_Count == 1) {
                            uniform->SetLocation(GetUniformLocation(uniform->m_Name));
                        }
                        else {
                            for (int n = 0; n < uniform->m_Count; n++) {
                                uniform->SetLocation(GetUniformLocation(uniform->m_Name + "[" + std::to_string(n) + "]"));
                            }
                        }
                    }
                }
            }
        }

        // validate Fragment Shader Uniforms
        {
            const auto& decl = m_FragUniformGroup;
            if (decl) {
                const std::vector<ShaderUniformDeclaration*>& uniformList = decl->GetUniformDeclarations();
                for (int n = 0; n < uniformList.size(); n++) {
                    OpenGLShaderUniformDeclaration* uniform = (OpenGLShaderUniformDeclaration*)uniformList[n];

                    if (uniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT) {
                        // parse the struct fields
                        const ShaderStruct& s = *uniform->m_Struct;
                        const auto& fields = s.GetFields();
                        for (int k = 0; k < fields.size(); k++) {
                            OpenGLShaderUniformDeclaration* field = (OpenGLShaderUniformDeclaration*)fields[k];
                            field->SetLocation(GetUniformLocation(uniform->m_Name + "." + field->m_Name));
                        }
                    }
                    else {
                        uniform->SetLocation(GetUniformLocation(uniform->m_Name));
                    }
                }
            }
        }

        // validate Sampler uniforms
        u32 samplerID = 0;
        for (int n = 0; n < m_Samplers.size(); n++) {
            OpenGLShaderSamplerDeclaration* sampler = (OpenGLShaderSamplerDeclaration*)m_Samplers[n];
            s32 loc = GetUniformLocation(sampler->m_Name);

            if (sampler->GetCount() == 1) {
                sampler->m_ID = samplerID;
                if (loc != -1)
                    glUniform1i(loc, samplerID);

                samplerID++;
            }
            else if (sampler->GetCount() > 1) {
                sampler->m_ID = 0;
                uint32_t count = sampler->GetCount();
                int* samplers = new int[count];
                for (uint32_t s = 0; s < count; s++)
                    samplers[s] = s;
                s32 loc2 = GetUniformLocation(sampler->GetName());
                glUniform1iv(loc2, count, samplers);
                delete[] samplers;
            }
        }
    }

    std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source) {
        std::unordered_map<GLenum, std::string> shaderSources;

        const char* typeToken = "#type";
        size_t typeTokenLength = strlen(typeToken);
        size_t pos = source.find(typeToken, 0);
        while (pos != std::string::npos) {
            size_t eol = source.find_first_of("\r\n", pos);
            ENGINE_LOG_ASSERT(eol != std::string::npos, "Syntax error");
            size_t begin = pos + typeTokenLength + 1;
            std::string type = source.substr(begin, eol - begin);
            ENGINE_LOG_ASSERT(type == "vertex" || type == "fragment" || type == "pixel" || type == "geometry", "Invalid shader type specified");

            size_t nextLinePos = source.find_first_not_of("\r\n", eol);
            pos = source.find(typeToken, nextLinePos);
            shaderSources[ShaderTypeFromString(type)] = 
                source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
        }

        return shaderSources;
    }

    s32 OpenGLShader::GetUniformLocation(const std::string & name) const
    {
        s32 loc = glGetUniformLocation(m_ShaderID, name.c_str());
        if (loc == -1) {
            //ENGINE_LOG_WARN("Could not find uniform '{0}' in shader", name);
        }

        return loc;
    }

    void OpenGLShader::Compile() {
        GLuint program = glCreateProgram();
        
        //ENGINE_LOG_ASSERT(m_ShaderSources.size() <= 2, "More than two shader programs not supported");
        std::array<GLenum, 3> glShaderIDs;
        int idx = 0;

        for (auto& kv : m_ShaderSources) {
            GLenum type = kv.first;
            const std::string& source = kv.second;

            GLuint shader = glCreateShader(type);

            const char *sourceCStr = source.c_str();
            glShaderSource(shader, 1, &sourceCStr, 0);
            glCompileShader(shader);

            GLint isCompiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
            if (isCompiled == GL_FALSE)
            {
                GLint maxLength = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

                std::vector<GLchar> infoLog(maxLength);
                glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

                glDeleteShader(shader);

                ENGINE_LOG_ERROR("{0}", infoLog.data());
                ENGINE_LOG_ASSERT(false, "Shader failed to compile");

                return;
            }
            glAttachShader(program, shader);
            glShaderIDs[idx++] = shader;
        }

        // Link our program
        glLinkProgram(program);

        // Note the different functions here: glGetProgram* instead of glGetShader*.
        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

            // We don't need the program anymore.
            glDeleteProgram(program);
            
            for (auto id : glShaderIDs) {
                glDeleteShader(id);
            }

            ENGINE_LOG_ERROR("{0}", infoLog.data());
            ENGINE_LOG_ASSERT(false, "Shader failed to link");
            return;
        }

        // Always detach shaders after a successful link.
        for (auto id : glShaderIDs) {
            glDetachShader(program, id);
        }

        m_ShaderID = program;
    }

    std::string OpenGLShader::ReadFile(const std::string& path) {
        std::string result;
        std::ifstream in(path, std::ios::in | std::ios::binary);
        if (in) {
            in.seekg(0, std::ios::end);
            result.resize(in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(&result[0], result.size());
            in.close();
        }
        else {
            ENGINE_LOG_ERROR("Could not open file: {0}", path);
        }
        return result;
    }

    void OpenGLShader::Bind() const {
        glUseProgram(m_ShaderID);
    }

    void OpenGLShader::UnBind() const {
        glUseProgram(0);
    }

    // Parsing helper functions
    const char* FindToken(const char* str, const std::string& token)
    {
        const char* t = str;
        while (t = strstr(t, token.c_str()))
        {
            bool left = str == t || isspace(t[-1]);
            bool right = !t[token.size()] || isspace(t[token.size()]);
            if (left && right)
                return t;

            t += token.size();
        }
        return nullptr;
    }

    std::string GetStatement(const char* str, const char** outPosition)
    {
        const char* end = strstr(str, ";");
        if (!end)
            return str;

        if (outPosition)
            *outPosition = end;
        uint32_t length = end - str + 1;
        return std::string(str, length);
    }

    std::string GetBlock(const char* str, const char** outPosition)
    {
        const char* end = strstr(str, "}");
        if (!end)
            return str;

        if (outPosition)
            *outPosition = end;
        uint32_t length = end - str + 1;
        return std::string(str, length);
    }

    std::vector<std::string> SplitString(const std::string& string, const std::string& delimiters) {
        size_t start = 0;
        size_t end = string.find_first_of(delimiters);

        std::vector<std::string> result;

        while (end <= std::string::npos)
        {
            std::string token = string.substr(start, end - start);
            if (!token.empty())
                result.push_back(token);

            if (end == std::string::npos)
                break;

            start = end + 1;
            end = string.find_first_of(delimiters, start);
        }

        return result;
    }

    ShaderStruct* OpenGLShader::FindStruct(const std::string& structName) const {
        for (ShaderStruct* s : m_Structs)
        {
            if (s->GetName() == structName)
                return s;
        }
        return nullptr;
    }

    void OpenGLShader::ParseUniform(const std::string& statement, ShaderDomain domain) {
        ENGINE_LOG_TRACE("Shader parsing uniform statement: ({0})", statement);
        // read the uniform and determine how much data to allocate in the buffer

        // split by all whitespace and newline characters
        std::vector<std::string> tokens = SplitString(statement, " \t\r\n");
        
        uint32_t index = 0;

        // tokens[0] should be uniform
        // tokens[1] should be the type (float, vec3, mat4, etc...)
        // tokens[2] should be the variable name (u_color) (still has the ; at the end)

        ENGINE_LOG_ASSERT(tokens[0] == "uniform", "First token should be 'uniform'");
        std::string typeString = tokens[1];
        ENGINE_LOG_ASSERT(tokens[2][tokens[2].size()-1] == ';', "Statement should end in a ';'");
        std::string varName = tokens[2].substr(0, tokens[2].size() - 1);

        // if uniform is an array, it still hase the [x] at the end of the varName
        std::string varName_tmp(varName);
        int32_t arrCount = 1;
        const char* namestr_tmp = varName_tmp.c_str();
        if (const char* s = strstr(namestr_tmp, "["))
        {
            varName = std::string(namestr_tmp, s - namestr_tmp);

            const char* end = strstr(namestr_tmp, "]");
            std::string c(s + 1, end - s);
            arrCount = atoi(c.c_str());
        }

        if (typeString == "sampler2D" || typeString == "samplerCube") {
            ShaderSamplerDeclaration* decl = new OpenGLShaderSamplerDeclaration(OpenGLShaderSamplerDeclaration::StringToType(typeString), varName, arrCount);
            m_Samplers.push_back(decl);
        }
        else {
            // Can now store a uniform of type typeString, called varName, with a count of arrCount
            OpenGLShaderUniformDeclaration::Type type = OpenGLShaderUniformDeclaration::StringToType(typeString);
            OpenGLShaderUniformDeclaration* decl = nullptr;

            if (type == OpenGLShaderUniformDeclaration::Type::NONE) {
                // has to be a struct uniform
                ShaderStruct* sStruct = FindStruct(typeString);
                ENGINE_LOG_ASSERT(sStruct, "Cant find struct!");
                decl = new OpenGLShaderUniformDeclaration(domain, sStruct, varName, arrCount);
            }
            else {
                // needs to get deleted at some point...
                decl = new OpenGLShaderUniformDeclaration(domain, type, varName, arrCount);
            }

            if (varName.find("r_") == 0) {
                // starts with r_, reserve it as a render-wide uniform
            }
            else {
                if (domain == ShaderDomain::Vertex) {
                    if (!m_VertexUniformGroup)
                        m_VertexUniformGroup.reset(new OpenGLShaderUniformGroupDeclaration("vertShaderGroup", domain));
                    m_VertexUniformGroup->AddUniform(decl);
                }
                else if (domain == ShaderDomain::Fragment) {
                    if (!m_FragUniformGroup)
                        m_FragUniformGroup.reset(new OpenGLShaderUniformGroupDeclaration("fragShaderGroup", domain));
                    m_FragUniformGroup->AddUniform(decl);
                }
            }
        }
    }

    void OpenGLShader::ParseUniformStruct(const std::string& block, ShaderDomain domain) {
        ENGINE_LOG_TRACE("Shader found token struct: ({0})", block);
        // read the uniform and determine how much data to allocate in the buffer

        // split by all whitespace and newline characters
        std::vector<std::string> tokens = SplitString(block, " \t\r\n");

        // tokens[0] should be struct
        // tokens[1] should be the struct name
        // tokens[2] should be the {
        // tokens[3-n] should be indiviual statements
        // last token should be }

        ENGINE_LOG_ASSERT(tokens[0] == "struct", "First token should be 'struct'");
        std::string structName = tokens[1];
        ENGINE_LOG_ASSERT(tokens[2] == "{", "Should be a space between struct name and {");

        // start building a ShaderStruct entry
        ShaderStruct* uniformStruct = new ShaderStruct(structName);

        u32 index = 3;
        while (index < tokens.size()) {
            if (tokens[index] == "}") // end of struct decl
                break;

            std::string typeString = tokens[index++];
            std::string varName = tokens[index++];

            if (const char* s = strstr(varName.c_str(), ";"))
                varName = std::string(varName.c_str(), s - varName.c_str());

            // get count if an array
            uint32_t arrCount = 1;
            const char* namestr = varName.c_str();
            if (const char* s = strstr(namestr, "["))
            {
                varName = std::string(namestr, s - namestr);

                const char* end = strstr(namestr, "]");
                std::string c(s + 1, end - s);
                arrCount = atoi(c.c_str());
            }

            // now have an individual var to add to the struct declaration
            ShaderUniformDeclaration* field = new OpenGLShaderUniformDeclaration(domain, OpenGLShaderUniformDeclaration::StringToType(typeString), varName, arrCount);
            uniformStruct->AddField(field);
        }
        m_Structs.push_back(uniformStruct);

        bool done = true;
    }

    OpenGLShaderUniformGroupDeclaration::OpenGLShaderUniformGroupDeclaration(const std::string& name, ShaderDomain domain) 
        : m_Name(name), m_Domain(domain), m_Size(0) {

    }

    OpenGLShaderUniformGroupDeclaration::~OpenGLShaderUniformGroupDeclaration() {
        for (auto uniform : m_Uniforms) {
            auto uniCast = static_cast<OpenGLShaderUniformDeclaration*>(uniform);

            if (uniCast->m_Struct) {
                auto s = uniCast->m_Struct;

                s->Cleanup();
            }

            delete uniCast;
            uniCast = nullptr;
        }
    }

    OpenGLShaderUniformDeclaration::OpenGLShaderUniformDeclaration(ShaderDomain domain, Type type, const std::string& name, u32 count) 
        : m_Domain(domain), m_Type(type), m_Name(name), m_Count(count), m_Struct(nullptr) {
        m_Size = SizeOfUniformType(type) * count;
    }

    OpenGLShaderUniformDeclaration::OpenGLShaderUniformDeclaration(ShaderDomain domain, ShaderStruct* sStruct, const std::string& name, u32 count)
        : m_Domain(domain), m_Struct(sStruct), m_Type(OpenGLShaderUniformDeclaration::Type::STRUCT), m_Name(name), m_Count(count) {
        m_Size = sStruct->GetSize() * count;
        m_Size = sStruct->GetSize() * count;
    }

    void OpenGLShaderUniformDeclaration::SetOffset(u32 offset) {
        m_Offset = offset;
    }

    OpenGLShaderUniformDeclaration::Type OpenGLShaderUniformDeclaration::StringToType(const std::string& type) {
        if (type == "int")      return Type::INT32;
        if (type == "float")    return Type::FLOAT32;
        if (type == "vec2")     return Type::VEC2;
        if (type == "vec3")     return Type::VEC3;
        if (type == "vec4")     return Type::VEC4;
        if (type == "mat3")     return Type::MAT3;
        if (type == "mat4")     return Type::MAT4;

        return Type::NONE;
    }

    u32 OpenGLShaderUniformDeclaration::SizeOfUniformType(Type type) {
        switch (type)
        {
        case OpenGLShaderUniformDeclaration::Type::INT32:       return 4;
        case OpenGLShaderUniformDeclaration::Type::FLOAT32:     return 4;
        case OpenGLShaderUniformDeclaration::Type::VEC2:        return 4 * 2;
        case OpenGLShaderUniformDeclaration::Type::VEC3:        return 4 * 3;
        case OpenGLShaderUniformDeclaration::Type::VEC4:        return 4 * 4;
        case OpenGLShaderUniformDeclaration::Type::MAT3:        return 4 * 3 * 3;
        case OpenGLShaderUniformDeclaration::Type::MAT4:        return 4 * 4 * 4;
        }
        return 0;
    }

    ShaderUniformDeclaration * OpenGLShaderUniformGroupDeclaration::FindUniformDeclaration(const std::string & name) const {
        for (auto uniform : m_Uniforms) {
            if (uniform->GetName() == name)
                return uniform;
        }
        return nullptr;
    }
    void OpenGLShaderUniformGroupDeclaration::AddUniform(OpenGLShaderUniformDeclaration * uniform) {
        u32 offset = 0;
        if (m_Uniforms.size()) {
            // there are already uniforms in the group
            OpenGLShaderUniformDeclaration* previous = (OpenGLShaderUniformDeclaration*)m_Uniforms.back();
            offset = previous->m_Offset + previous->m_Size;
        }
        uniform->SetOffset(offset);
        m_Size += uniform->GetSize();
        m_Uniforms.push_back(uniform);
    }

    void OpenGLShader::IdentifyUniforms() {
        auto& vertexSource = m_ShaderSources[GL_VERTEX_SHADER];
        auto& fragmentSource = m_ShaderSources[GL_FRAGMENT_SHADER];

        const char* token;
        const char* vstr;
        const char* fstr;

        vstr = vertexSource.c_str();
        while (token = FindToken(vstr, "struct")) {
            ParseUniformStruct(GetBlock(token, &vstr), ShaderDomain::Vertex);
        }

        vstr = vertexSource.c_str();
        while (token = FindToken(vstr, "uniform")) {
            ParseUniform(GetStatement(token, &vstr), ShaderDomain::Vertex);
        }


        fstr = fragmentSource.c_str();
        while (token = FindToken(fstr, "struct")) {
            ParseUniformStruct(GetBlock(token, &fstr), ShaderDomain::Fragment);
        }

        fstr = fragmentSource.c_str();
        while (token = FindToken(fstr, "uniform")) {
            ParseUniform(GetStatement(token, &fstr), ShaderDomain::Fragment);
        }

    }

    void OpenGLShader::SetVertUniformBuffer(Buffer buffer) {
        glUseProgram(m_ShaderID);
        ResolveAndSetUniforms(m_VertexUniformGroup, buffer);
    }

    void OpenGLShader::SetFragUniformBuffer(Buffer buffer) {
        glUseProgram(m_ShaderID);
        ResolveAndSetUniforms(m_FragUniformGroup, buffer);
    }

    void OpenGLShader::ResolveAndSetUniforms(const Ref<OpenGLShaderUniformGroupDeclaration>& decl, Buffer buffer)
    {
        const std::vector<ShaderUniformDeclaration*>& uniforms = decl->GetUniformDeclarations();
        for (size_t i = 0; i < uniforms.size(); i++)
        {
            OpenGLShaderUniformDeclaration* uniform = (OpenGLShaderUniformDeclaration*)uniforms[i];
            if (uniform->m_Count > 1)
                ResolveAndSetUniformArray(uniform, buffer);
            else
                ResolveAndSetUniform(uniform, buffer);
        }
    }

    void OpenGLShader::ResolveAndSetUniform(OpenGLShaderUniformDeclaration* uniform, Buffer buffer)
    {
        if (uniform->GetLocation() == -1)
            return;

        ENGINE_LOG_ASSERT(uniform->GetLocation() != -1, "Uniform has invalid location!");

        uint32_t offset = uniform->GetOffset();
        switch (uniform->GetType())
        {
        case OpenGLShaderUniformDeclaration::Type::FLOAT32:
            UploadUniformFloat(uniform->GetLocation(), *(float*)&buffer.Data[offset]);
            break;
        case OpenGLShaderUniformDeclaration::Type::INT32:
            UploadUniformInt(uniform->GetLocation(), *(int32_t*)&buffer.Data[offset]);
            break;
        case OpenGLShaderUniformDeclaration::Type::VEC2:
            UploadUniformFloat2(uniform->GetLocation(), *(math::vec2*)&buffer.Data[offset]);
            break;
        case OpenGLShaderUniformDeclaration::Type::VEC3:
            UploadUniformFloat3(uniform->GetLocation(), *(math::vec3*)&buffer.Data[offset]);
            break;
        case OpenGLShaderUniformDeclaration::Type::VEC4:
            UploadUniformFloat4(uniform->GetLocation(), *(math::vec4*)&buffer.Data[offset]);
            break;
        case OpenGLShaderUniformDeclaration::Type::MAT3:
            UploadUniformMat3(uniform->GetLocation(), *(math::mat3*)&buffer.Data[offset]);
            break;
        case OpenGLShaderUniformDeclaration::Type::MAT4:
            UploadUniformMat4(uniform->GetLocation(), *(math::mat4*)&buffer.Data[offset]);
            break;
        case OpenGLShaderUniformDeclaration::Type::STRUCT:
            UploadUniformStruct(uniform, buffer.Data, offset);
            break;
        default:
            ENGINE_LOG_ASSERT(false, "Unknown uniform type!");
        }
    }

    void OpenGLShader::ResolveAndSetUniformArray(OpenGLShaderUniformDeclaration* uniform, Buffer buffer)
    {
        //HZ_CORE_ASSERT(uniform->GetLocation() != -1, "Uniform has invalid location!");

        uint32_t offset = uniform->GetOffset();
        switch (uniform->GetType())
        {
        case OpenGLShaderUniformDeclaration::Type::FLOAT32:
            UploadUniformFloat(uniform->GetLocation(), *(float*)&buffer.Data[offset]);
            break;
        case OpenGLShaderUniformDeclaration::Type::INT32:
            UploadUniformInt(uniform->GetLocation(), *(int32_t*)&buffer.Data[offset]);
            break;
        case OpenGLShaderUniformDeclaration::Type::VEC2:
            UploadUniformFloat2(uniform->GetLocation(), *(math::vec2*)&buffer.Data[offset]);
            break;
        case OpenGLShaderUniformDeclaration::Type::VEC3:
            UploadUniformFloat3(uniform->GetLocation(), *(math::vec3*)&buffer.Data[offset]);
            break;
        case OpenGLShaderUniformDeclaration::Type::VEC4:
            UploadUniformFloat4(uniform->GetLocation(), *(math::vec4*)&buffer.Data[offset]);
            break;
        case OpenGLShaderUniformDeclaration::Type::MAT3:
            UploadUniformMat3(uniform->GetLocation(), *(math::mat3*)&buffer.Data[offset]);
            break;
        case OpenGLShaderUniformDeclaration::Type::MAT4:
            UploadUniformMat4Array(uniform->GetLocation(), *(math::mat4*)&buffer.Data[offset], uniform->GetCount());
            break;
        case OpenGLShaderUniformDeclaration::Type::STRUCT:
            UploadUniformStruct(uniform, buffer.Data, offset);
            break;
        default:
            ENGINE_LOG_ASSERT(false, "Unknown uniform type!");
        }
    }

    void OpenGLShader::ResolveAndSetUniformField(const OpenGLShaderUniformDeclaration& field, byte* data, int32_t offset)
    {
        switch (field.GetType())
        {
        case OpenGLShaderUniformDeclaration::Type::FLOAT32:
            UploadUniformFloat(field.GetLocation(), *(float*)&data[offset]);
            break;
        case OpenGLShaderUniformDeclaration::Type::INT32:
            UploadUniformInt(field.GetLocation(), *(int32_t*)&data[offset]);
            break;
        case OpenGLShaderUniformDeclaration::Type::VEC2:
            UploadUniformFloat2(field.GetLocation(), *(math::vec2*)&data[offset]);
            break;
        case OpenGLShaderUniformDeclaration::Type::VEC3:
            UploadUniformFloat3(field.GetLocation(), *(math::vec3*)&data[offset]);
            break;
        case OpenGLShaderUniformDeclaration::Type::VEC4:
            UploadUniformFloat4(field.GetLocation(), *(math::vec4*)&data[offset]);
            break;
        case OpenGLShaderUniformDeclaration::Type::MAT3:
            UploadUniformMat3(field.GetLocation(), *(math::mat3*)&data[offset]);
            break;
        case OpenGLShaderUniformDeclaration::Type::MAT4:
            UploadUniformMat4(field.GetLocation(), *(math::mat4*)&data[offset]);
            break;
        default:
            ENGINE_LOG_ASSERT(false, "Unknown uniform type!");
        }
    }

    OpenGLShaderSamplerDeclaration::OpenGLShaderSamplerDeclaration(Type type, const std::string & name, uint32_t count) 
        : m_Type(type), m_Name(name), m_Count(count) {
    }

    OpenGLShaderSamplerDeclaration::Type OpenGLShaderSamplerDeclaration::StringToType(const std::string & type) {
        if (type == "sampler2D")    return Type::TEXTURE2D;
        if (type == "samplerCube")  return Type::TEXTURECUBE;

        return Type::NONE;
    }

    std::string OpenGLShaderSamplerDeclaration::TypeToString(Type type) {
        switch (type)
        {
        case Type::TEXTURE2D:       return "sampler2D";
        case Type::TEXTURECUBE:     return "samplerCube";
        }
        return "Error: Invalid Type";
    }

    void OpenGLShader::UploadUniformInt(uint32_t location, int32_t value)
    {
        glUniform1i(location, value);
    }

    void OpenGLShader::UploadUniformIntArray(uint32_t location, int32_t* values, int32_t count)
    {
        glUniform1iv(location, count, values);
    }

    void OpenGLShader::UploadUniformFloat(uint32_t location, float value)
    {
        glUniform1f(location, value);
    }

    void OpenGLShader::UploadUniformFloat2(uint32_t location, const math::vec2& value)
    {
        glUniform2f(location, value.x, value.y);
    }

    void OpenGLShader::UploadUniformFloat3(uint32_t location, const math::vec3& value)
    {
        glUniform3f(location, value.x, value.y, value.z);
    }

    void OpenGLShader::UploadUniformFloat4(uint32_t location, const math::vec4& value)
    {
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }

    void OpenGLShader::UploadUniformMat3(uint32_t location, const math::mat3& value)
    {
        glUniformMatrix3fv(location, 1, GL_FALSE, &(value._11));
    }

    void OpenGLShader::UploadUniformMat4(uint32_t location, const math::mat4& value)
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, &(value._11));
    }

    void OpenGLShader::UploadUniformMat4Array(uint32_t location, const math::mat4& values, uint32_t count)
    {
        glUniformMatrix4fv(location, count, GL_FALSE, &(values._11));
    }

    void OpenGLShader::UploadUniformStruct(OpenGLShaderUniformDeclaration* uniform, byte* buffer, uint32_t offset)
    {
        const ShaderStruct& s = *uniform->m_Struct;
        const auto& fields = s.GetFields();
        for (size_t k = 0; k < fields.size(); k++)
        {
            OpenGLShaderUniformDeclaration* field = (OpenGLShaderUniformDeclaration*)fields[k];
            ResolveAndSetUniformField(*field, buffer, offset);
            offset += field->m_Size;
        }
    }

    //TODO: turn this logging back on
    void OpenGLShader::SetMat4(const std::string &name, const math::mat4& value) const {
        s32 loc = glGetUniformLocation(m_ShaderID, name.c_str());
        if (loc == -1) {
            //ENGINE_LOG_WARN("Chould not find uniform: {0}", name.c_str());
            return;
        }
        glUniformMatrix4fv(loc, 1, GL_FALSE, &(value._11));

    }

    void OpenGLShader::SetVec3(const std::string &name, const math::vec3& value) const {
        s32 loc = glGetUniformLocation(m_ShaderID, name.c_str());
        if (loc == -1) {
            //ENGINE_LOG_WARN("Chould not find uniform: {0}", name.c_str());
            return;
        }
        glUniform3f(loc, value.x, value.y, value.z);
    }

    void OpenGLShader::SetVec4(const std::string &name, const math::vec4& value) const {
        s32 loc = glGetUniformLocation(m_ShaderID, name.c_str());
        if (loc == -1) {
            //ENGINE_LOG_WARN("Chould not find uniform: {0}", name.c_str());
            return;
        }
        glUniform4f(loc, value.x, value.y, value.z, value.w);
    }

    void OpenGLShader::SetFloat(const std::string &name, f32 value) const {
        s32 loc = glGetUniformLocation(m_ShaderID, name.c_str());
        if (loc == -1) {
            //ENGINE_LOG_WARN("Chould not find uniform: {0}", name.c_str());
            return;
        }
        glUniform1f(loc, value);
    }

    void OpenGLShader::SetInt(const std::string &name, s32 value) const {
        s32 loc = glGetUniformLocation(m_ShaderID, name.c_str());
        if (loc == -1) {
            //ENGINE_LOG_WARN("Chould not find uniform: {0}", name.c_str());
            return;
        }
        glUniform1i(loc, value);
    }
}