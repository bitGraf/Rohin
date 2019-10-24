#ifndef SHADER_H
#define SHADER_H

#include "glad\glad.h"

#include "DataTypes.hpp"

#include <string>

class Shader {
public:

    Shader(std::string filename);
    ~Shader();

    void create(std::string filename);

    /* Set uniforms */
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    //void setMat4(const std::string &name, mat4* value) const;
    //void setvec3(const std::string &name, vec3* value) const;
    void setVec4(const std::string &name, float a, float b, float c, float d) const;

private:
    GLuint glShaderID;

    void addVertexShader(std::string filename);
    void addFragmentShader(std::string filename);

    void compile();
};

#endif