#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

#include <unordered_map>

#include "CoreSystem.hpp"
#include "Shader.hpp"
#include "SceneManager.hpp"

namespace ShaderList {
    const stringID mainShader   = "mainShader"_sid;
    const stringID shader2      = "shader2"_sid;
    const stringID shader3      = "shader3"_sid;
    const stringID shader4      = "shader4"_sid;
}

class RenderManager : public CoreSystem {
public:
    RenderManager();
    ~RenderManager();

    void update(double dt);
    void handleMessage(Message msg);
    void destroy();
    void sys_create(ConfigurationManager* configMgr);
        
    /* System Unique functions */
    void setShader(stringID id);
    void renderScene(Scene* scene);

private:
    Shader m_shader;

    void setCamera(Camera* camera);
    void setCurrentMesh(const TriangleMesh* mesh);
    void setCurrentMaterial(const Material* material);
    void setTransforms(const math::vec3* pos, const math::mat3* orientation, const math::vec3* scale);
    void renderPrimitive(const TriangleMesh* mesh);
};

#endif
