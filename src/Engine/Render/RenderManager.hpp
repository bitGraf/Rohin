#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

#include <unordered_map>

#include "Message/CoreSystem.hpp"
#include "Shader.hpp"
#include "Scene/SceneManager.hpp"
#include "Window/Framebuffer.hpp"
#include "Window/Window.hpp"
#include "DynamicFont.hpp"
#include "Shadowmap.hpp"
#include "Resource\Texture.hpp"

#include <stdio.h>
#include <stdlib.h>

class RenderManager : public CoreSystem {
public:
    RenderManager();
    ~RenderManager();

    void update(double dt);
    void handleMessage(Message msg);
    void destroy();
    CoreSystem* create();
        
    /* System Unique functions */
    void renderScene(Window* window, Scene* scene);
    void renderEditor(Window* window, Scene* scene);
    void lastFrameTime(long long micro);

private:
    Shader m_mainShader, m_lineShader,
        m_skyboxShader, m_fullscreenShader, 
        m_shadowShader, m_volumeShader;

    Framebuffer fb;
    Shadowmap sm;
    Texture blackTex, whiteTex, normalTex;

    GLuint fullscreenVAO;

    DynamicFont font;
    f32 fps;

    void setCamera(Shader* shader, Camera* camera);
    void setCurrentMesh(const TriangleMesh* mesh);
    void setCurrentMaterial(Shader* shader, const Material* material);
    void setTransforms(Shader* shader, const math::vec3* pos, const math::mat3* orientation, const math::vec3* scale);
    void renderPrimitive(const TriangleMesh* mesh);
    void renderGrid(Shader* shader, GLuint vao, GLuint numVerts);
    void renderSkybox(Shader* shader, Camera* camera, EnvironmentMap* skybox);
};

#endif
