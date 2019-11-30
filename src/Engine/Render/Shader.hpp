#ifndef SHADER_H
#define SHADER_H

#include "glad\glad.h"

#include <stdio.h>
#include <string>
#include <iostream>
#include <chrono>

#include "Scene/Material.hpp"
#include "GameObject/Light.hpp"

const std::string ShaderResourcePath = "Data/shaders/";

class Shader {
public:

    Shader();
    ~Shader();

    void create(std::string vShaderPath, std::string fShaderPath, std::string ShaderName);

    /* Set uniforms */
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, s32 value) const;
    void setFloat(const std::string &name, f32 value) const;

    void setVec2(const std::string &name, math::vec2 value) const;
    void setVec3(const std::string &name, math::vec3 value) const;
    void setVec4(const std::string &name, math::vec4 value) const;

    void setMat2(const std::string &name, math::mat2 value) const;
    void setMat3(const std::string &name, math::mat3 value) const;
    void setMat4(const std::string &name, math::mat4 value) const;

    void setMaterial(const std::string &name, const Material* mat) const;
    void setLights(
        const DirLight* sun,
        PointLight *pointLights[4],
        SpotLight *spotLights[4]) const;

    void use();

private:
    GLuint glShaderID;
};

#endif