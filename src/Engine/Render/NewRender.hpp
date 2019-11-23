#ifndef NEW_RENDER_H
#define NEW_RENDER_H

#include "GLFW\glfw3.h"
#include "Message\CoreSystem.hpp"
#include "Shader.hpp"
#include "Shadowmap.hpp"
#include "Resource\Texture.hpp"
#include "BatchRender.hpp"
#include "Window\Framebuffer.hpp"
#include "DynamicFont.hpp"
#include "Utils.hpp"

class BatchRenderer : public CoreSystem {
public:
    BatchRenderer();
    ~BatchRenderer();

    void update(double dt);
    void handleMessage(Message msg);
    void destroy();
    CoreSystem* create();

    /* System Unique functions */
    void renderBatch(BatchDrawCall* batch);

    // Render Passes
    void shadowPass(BatchDrawCall* batch);
    void staticPass(BatchDrawCall* batch);
    void dynamicPass(BatchDrawCall* batch);
    void skyboxPass(BatchDrawCall* batch);
    void lightVolumePass(BatchDrawCall* batch);
    void toneMap(BatchDrawCall* batch);
    void gammaCorrect(BatchDrawCall* batch);
    void renderDebug(BatchDrawCall* batch);

private:
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

    void beginProfile();
    _dur profileRenderPass();
    void endProfile();

    Shadowmap sm;
    Framebuffer_color_pos fb;
    //Framebuffer_swap swap_fb;
    Framebuffer fb_volume;
    Framebuffer fb_toneMap;
    Texture blackTex, whiteTex, normalTex, greenTex;

    GLuint fullscreenVAO;
    DynamicFont debugFont;


    Shader m_shadowPass;
    Shader m_staticPass;
    //Shader m_dynamicPass;
    Shader m_skyboxPass;
    Shader m_lightVolumePass;

    Shader m_toneMap;
    Shader m_gammaCorrect;
};

#endif
