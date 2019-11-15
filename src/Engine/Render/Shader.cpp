#include "Shader.hpp"

Shader::Shader() {
    glShaderID = 0;
}

Shader::~Shader() {

}

void Shader::create(
    std::string vShaderPath, 
    std::string fShaderPath,
    std::string ShaderName) {

    if (glShaderID) {
        // shader already exists, recreate it

        glDeleteShader(glShaderID);
    }

    auto startTime = std::chrono::system_clock::now();

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

    auto duration = std::chrono::system_clock::now() - startTime;

    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(duration).count() <<
        " Microseconds to compile." << std::endl;
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
    setInt(name + ".metallicRoughnessTexture", 2);
    setInt(name + ".occlusionTexture", 3);
    setInt(name + ".emissiveTexture", 4);

    setVec3(name + ".emissiveFactor", mat->emissiveFactor);
    setVec4(name + ".baseColorFactor", mat->baseColorFactor);
    setFloat(name + ".metallicFactor", mat->metallicFactor);
    setFloat(name + ".roughnessFactor", mat->roughnessFactor);
}

void Shader::setLights(
    const DirLight* sun, 
    PointLight* pointLights, 
    SpotLight* spotLights) const {
    using namespace math;

    /* Set Directional light */
    setVec3("sun.direction", sun->direction);
    setVec4("sun.color", sun->color);
    setFloat("sun.strength", sun->strength);

    /* Point Lights */
    for (int n = 0; n < 4; n++) {
        setVec3("pointLights[" + std::to_string(n) + 
            "].position", pointLights[n].position);
        setVec4("pointLights[" + std::to_string(n) +
            "].color", pointLights[n].color);
        setFloat("pointLights[" + std::to_string(n) +
            "].strength", pointLights[n].strength);
    }

    /* Spot Lights */
    for (int n = 0; n < 4; n++) {
        setVec3("spotLights[" + std::to_string(n) +
            "].position", spotLights[n].position);
        setVec3("spotLights[" + std::to_string(n) +
            "].direction", spotLights[n].direction);
        setVec4("spotLights[" + std::to_string(n) +
            "].color", spotLights[n].color);
        setFloat("spotLights[" + std::to_string(n) +
            "].strength", spotLights[n].strength);
        setFloat("spotLights[" + std::to_string(n) +
            "].inner_cutoff", spotLights[n].inner_cutoff);
        setFloat("spotLights[" + std::to_string(n) +
            "].outer_cutoff", spotLights[n].outer_cutoff);
    }
}

void Shader::use() {
    glUseProgram(glShaderID);
}