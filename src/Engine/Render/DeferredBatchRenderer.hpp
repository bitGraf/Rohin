#ifndef DEFERRED_RENDER_MANAGER_H
#define DEFERRED_RENDER_MANAGER_H

#include "GLFW/glfw3.h"
#include "Message/CoreSystem.hpp"
#include "Shader.hpp"
#include "Shadowmap.hpp"
#include "Resource/Texture.hpp"
#include "RenderBatch.hpp"
#include "Window/GBuffer.hpp"
#include "DynamicFont.hpp"
#include "Utils.hpp"
#include "Resource/ResourceManager.hpp"

class DeferredBatchRenderer : public CoreSystem {
public:
    DeferredBatchRenderer();
    ~DeferredBatchRenderer();

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

private:
    // Render Passes
    void geometryPass(RenderBatch* batch);
    void screenPass(RenderBatch* batch);

    using _clock = std::chrono::system_clock;
    using _time = std::chrono::system_clock::time_point;
    _time profileStart;

    using _dur = long long;
    _dur dur_fullRenderPass;
    MovingAverage<_dur, 100> avgRenderPass;
    _dur dur_geometryPass,
         dur_debug,
         dur_screenPass;

    void beginProfile();
    _dur profileRenderPass();
    void endProfile();

    GBuffer m_gBuffer;
    Texture blackTex, whiteTex, normalTex, greenTex;

    GLuint fullscreenVAO;
    DynamicFont debugFont;

    Shader m_geometryPassShader;
    Shader m_screenShader;

    Shader m_debugMeshShader;
    TriangleMesh* cameraMesh;

    u16 scr_width, scr_height;

    std::string soStr;
    u8 shaderOutput;
};

#endif
