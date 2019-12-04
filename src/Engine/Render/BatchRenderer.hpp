#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

#include "GLFW\glfw3.h"
#include "Message\CoreSystem.hpp"
#include "Shader.hpp"
#include "Shadowmap.hpp"
#include "Resource\Texture.hpp"
#include "RenderBatch.hpp"
#include "Window\Framebuffer.hpp"
#include "DynamicFont.hpp"
#include "Utils.hpp"
#include "Resource/ResourceManager.hpp"

class BatchRenderer : public CoreSystem {
public:
    BatchRenderer();
    ~BatchRenderer();

    void update(double dt);
    void handleMessage(Message msg);
    void destroy();
    CoreSystem* create();

    /* System Unique functions */
    void renderBatch(RenderBatch* batch);
    void renderDebug(RenderBatch* batch,
        double frameCount, long long lastFrame,
        bool debugMode);
    void loadResources(ResourceManager* resource);

    // Render Passes
    void shadowPass(RenderBatch* batch);
    void staticPass(RenderBatch* batch);
    void dynamicPass(RenderBatch* batch);
    void skyboxPass(RenderBatch* batch);
    void lightVolumePass(RenderBatch* batch);
    void toneMap(RenderBatch* batch);
    void pickPass(RenderBatch* batch);
    void gammaCorrect(RenderBatch* batch);

    UID_t pickObject(GLint x, GLint y);

private:
    void drawLine(vec3 A, vec3 B, vec3 colorA, vec3 colorB);
    void drawAABBB(vec3 center, vec3 min, vec3 max, vec3 color);

    using _clock = std::chrono::system_clock;
    using _time = std::chrono::system_clock::time_point;
    _time profileStart;

    using _dur = long long;
    _dur dur_fullRenderPass;
    MovingAverage<_dur, 100> avgRenderPass;
    _dur dur_shadowPass;
    _dur dur_staticPass;
    _dur dur_dynamicPass;
    _dur dur_skyboxPass;
    _dur dur_lightVolumePass;
    _dur dur_toneMap;
    _dur dur_gammaCorrect;
    _dur dur_pickPass;
    _dur dur_debug;

    void beginProfile();
    _dur profileRenderPass();
    void endProfile();

    Shadowmap sm;
    Framebuffer_color_pos fb;
    //Framebuffer_swap swap_fb;
    Framebuffer fb_volume;
    Framebuffer fb_toneMap;
    Framebuffer fb_pick;
    Texture blackTex, whiteTex, normalTex, greenTex;

    GLuint fullscreenVAO;
    DynamicFont debugFont;
    DynamicFont debugFontSmall;

    Shader m_pickPassShader;
    vec3 getUniqueColor(UID_t id);

    Shader m_shadowPass;
    Shader m_staticPass;
    //Shader m_dynamicPass;
    Shader m_skyboxPass;
    Shader m_lightVolumePass;

    Shader m_toneMap;
    Shader m_gammaCorrect;

    GLuint debugLineVAO;
    Shader m_debugLineShader;
    Shader m_debugMeshShader;
    TriangleMesh* cameraMesh;

    u16 scr_width, scr_height;
};

#endif
