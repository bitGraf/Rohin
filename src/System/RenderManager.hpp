#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

#include <unordered_map>

#include "CoreSystem.hpp"
#include "Render/Shader.hpp"
#include "SceneManager.hpp"

class RenderManager : public CoreSystem {
public:
    RenderManager();
    ~RenderManager();

    void update(double dt);
    void handleMessage(Message msg);
    void destroy();
    CoreSystem* create();
        
    /* System Unique functions */
    void setShader(stringID id);
    void renderScene(Scene* scene);

private:
    Shader m_mainShader, m_lineShader;

    void setCamera(Shader* shader, Camera* camera);
    void setCurrentMesh(const TriangleMesh* mesh);
    void setCurrentMaterial(Shader* shader, const Material* material);
    void setTransforms(Shader* shader, const math::vec3* pos, const math::mat3* orientation, const math::vec3* scale);
    void renderPrimitive(const TriangleMesh* mesh);
    void renderGrid(Shader* shader, GLuint vao, GLuint numVerts);
};

#endif
