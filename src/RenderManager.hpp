#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

#include <unordered_map>

#include "HashTable.hpp"
#include "Shader.hpp"

namespace ShaderList {
    const stringID mainShader   = "mainShader"_sid;
    const stringID shader2      = "shader2"_sid;
    const stringID shader3      = "shader3"_sid;
    const stringID shader4      = "shader4"_sid;
}

class RenderManager {
public:

    RenderManager();
    ~RenderManager();

    void create();
    void destroy();

    void setShader(stringID id);

private:
    std::unordered_map<stringID, Shader> shaders;
};

#endif
