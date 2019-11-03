#include "Shader.hpp"

Shader::Shader() {

}

Shader::~Shader() {

}

void Shader::create(
    std::string vShaderPath, 
    std::string fShaderPath,
    std::string ShaderName) {

    printf("Building shader(%s): [%s]|[%s]\n", 
        ShaderName.c_str(), vShaderPath.c_str(), fShaderPath.c_str());

    /* load shader files */
    FILE *f = fopen((ShaderResourcePath + vShaderPath).c_str(), "rb");
    if (f == NULL) {
        printf("  Failed to open file: %s\n", vShaderPath.c_str());
        return;
    }
    fseek(f, 0, SEEK_END);
    long vSize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *vShaderCode = (char *)malloc(vSize + 1);
    fread(vShaderCode, vSize, 1, f);
    fclose(f);
    vShaderCode[vSize] = 0;

    f = fopen((ShaderResourcePath + fShaderPath).c_str(), "rb");
    if (f == NULL) {
        printf("  Failed to open file: %s\n", fShaderPath.c_str());
        return;
    }
    fseek(f, 0, SEEK_END);
    long fSize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *fShaderCode = (char *)malloc(fSize + 1);
    fread(fShaderCode, fSize, 1, f);
    fclose(f);
    fShaderCode[fSize] = 0;

    //Build OpenGL Shader Program
    //Vertex shader
    GLuint vShader;
    vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vShaderCode, NULL);
    glCompileShader(vShader);
    //Check compile errors
    int  success;
    char infoLog[512];
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vShader, 512, NULL, infoLog);
        printf("  ERROR::SHADER::VERTEX::COMPILATION_FAILED(%s)\n %s\n", vShaderPath.c_str(), infoLog);
    }
    //Fragment shader
    GLuint fShader;
    fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fShaderCode, NULL);
    glCompileShader(fShader);
    //Check compile errors
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fShader, 512, NULL, infoLog);
        printf("  ERROR::SHADER::FRAGMENT::COMPILATION_FAILED(%s)\n %s\n", fShaderPath.c_str(), infoLog);
        system("pause");
    }
    //Shader program
    glShaderID = glCreateProgram();
    glAttachShader(glShaderID, vShader);
    glAttachShader(glShaderID, fShader);
    glLinkProgram(glShaderID);
    //Check errors
    glGetProgramiv(glShaderID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(glShaderID, 512, NULL, infoLog);
        printf("  ERROR::SHADER::LINK_FAILED\n %s\n", infoLog);
        system("pause");
    }

    free(vShaderCode);
    free(fShaderCode);
    glDeleteShader(vShader);
    glDeleteShader(fShader);
}

void Shader::setBool(const std::string &name, bool value) const {
    GLuint loc = glGetUniformLocation(glShaderID, name.c_str());
    glUniform1i(loc, (s32)value);
}

void Shader::setInt(const std::string &name, s32 value) const {
    GLuint loc = glGetUniformLocation(glShaderID, name.c_str());
    glUniform1i(loc, value);
}

void Shader::setFloat(const std::string &name, f32 value) const {
    GLuint loc = glGetUniformLocation(glShaderID, name.c_str());
    glUniform1f(loc, value);
}


void Shader::setVec2(const std::string &name, math::vec2 value) const {
    GLuint loc = glGetUniformLocation(glShaderID, name.c_str());
    glUniform2f(loc, value.x, value.y);
}
void Shader::setVec3(const std::string &name, math::vec3 value) const {
    GLuint loc = glGetUniformLocation(glShaderID, name.c_str());
    glUniform3f(loc, value.x, value.y, value.z);
}
void Shader::setVec4(const std::string &name, math::vec4 value) const {
    GLuint loc = glGetUniformLocation(glShaderID, name.c_str());
    glUniform4f(loc, value.x, value.y, value.z, value.w);
}

void Shader::setMat2(const std::string &name, math::mat2 value) const {
    GLuint loc = glGetUniformLocation(glShaderID, name.c_str());
    glUniformMatrix2fv(loc, 1, GL_FALSE, &(value._11));
}
void Shader::setMat3(const std::string &name, math::mat3 value) const {
    GLuint loc = glGetUniformLocation(glShaderID, name.c_str());
    glUniformMatrix3fv(loc, 1, GL_FALSE, &(value._11));
}
void Shader::setMat4(const std::string &name, math::mat4 value) const {
    GLuint loc = glGetUniformLocation(glShaderID, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, &(value._11));
}

void Shader::setMaterial(const std::string &name, const Material* mat) const {
    setInt(name + ".baseColorTexture", 0);
    setInt(name + ".normalTexture", 1);
    setInt(name + ".occlusionTexture", 2);
    setInt(name + ".metallicRoughnessTexture", 3);
    setInt(name + ".emissiveTexture", 4);

    setVec3(name + ".emissiveFactor", mat->emissiveFactor);
    setVec4(name + ".baseColorFactor", mat->baseColorFactor);
    setFloat(name + ".metallicFactor", mat->metallicFactor);
    setFloat(name + ".roughnessFactor", mat->roughnessFactor);
}

void Shader::setLights() const {
    using namespace math;

    setVec3("pointLights[0].position", vec3(0,5,0));
    setVec4("pointLights[0].color", vec4(1,0,0,1));
    setFloat("pointLights[0].strength", 10);

    setVec3("pointLights[1].position", vec3(0, 0, 0));
    setVec4("pointLights[1].color", vec4(0, 0, 0, 1));
    setFloat("pointLights[1].strength", 0);

    setVec3("pointLights[2].position", vec3(0, 0, 0));
    setVec4("pointLights[2].color", vec4(0, 0, 0, 1));
    setFloat("pointLights[2].strength", 0);

    setVec3("pointLights[3].position", vec3(0, 0, 0));
    setVec4("pointLights[3].color", vec4(0, 0, 0, 1));
    setFloat("pointLights[3].strength", 0);

    setVec3("sun.direction", vec3(-1, -1, -1));
    setVec4("sun.color", vec4(.8, 1, 1, 1));
    setFloat("sun.strength", 10);

    setVec3("spotLight.position", vec3(0, 0, 0));
    setVec3("spotLight.direction", vec3(0, 0, 0));
    setVec4("spotLight.color", vec4(0, 0, 0, 1));
    setFloat("spotLight.strength", 0);
    setFloat("spotLight.cutoff", .1);
}

void Shader::use() {
    glUseProgram(glShaderID);
}