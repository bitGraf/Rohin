#include "DeferredBatchRenderer.hpp"
#include "Scene/Scene.hpp"

DeferredBatchRenderer::DeferredBatchRenderer() :
    shaderOutput(0),
    soStr("Albedo"),
    ssaoKernel(Kernel_Size),
    ssaoFBO(800, 600),
    m_gBuffer(800, 600),
    ssaoBlurFBO(800, 600)
{}

DeferredBatchRenderer::~DeferredBatchRenderer() {}

void DeferredBatchRenderer::update(double dt) {
}

void DeferredBatchRenderer::destroy() {}
/*
CoreSystem* DeferredBatchRenderer::create() {
    m_geometryPassShader.create("pipeline/geometryPass.vert", "pipeline/geometryPass.frag", "geometryPassShader");
    m_ssaoPassShader.create("pipeline/screen.vert", "pipeline/ssao.frag", "ssaoShader");
    m_ssaoBlurShader.create("pipeline/screen.vert", "pipeline/ssaoBlur.frag", "ssaoBlurShader");
    m_screenShader.create("pipeline/screen.vert", "pipeline/screen.frag", "screenShader");

    m_debugMeshShader.create("debugMesh.vert", "debugMesh.frag", "debugMeshShader");

    blackTex.loadImage("black.png");
    whiteTex.loadImage("white.png");
    normalTex.loadImage("normal.png");
    greenTex.loadImage("green.png");

    scr_width = DEFAULT_SCREEN_WIDTH;
    scr_height = DEFAULT_SCREEN_HEIGHT;

    //m_gBuffer.create(scr_width, scr_height);
    m_gBuffer.addColorBufferObject("Target0", 0, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
    m_gBuffer.addColorBufferObject("Target1", 1, GL_RGBA16F, GL_RGBA, GL_FLOAT);
    m_gBuffer.addColorBufferObject("Target2", 2, GL_RGBA16F, GL_RGBA, GL_FLOAT);
    m_gBuffer.addColorBufferObject("TargetDepth", 3, GL_R32F, GL_RED, GL_FLOAT);
    m_gBuffer.addColorBufferObject("TargetPos", 4, GL_RGB32F, GL_RGB, GL_FLOAT);
    m_gBuffer.addRenderBufferObject();
    m_gBuffer.create();
    m_gBuffer.resize(scr_width, scr_height);

	spriteRender.initRenderData();

    debugFont.InitTextRendering();
    debugFont.create("UbuntuMono-Regular.ttf", 16, scr_width, scr_height);

    const float fullscreenVerts[] = {
        -1, -1,
        -1,  1,
        1, -1,

        1, -1,
        -1,  1,
        1,  1
    };

    GLuint fullscreenVBO;
    glGenVertexArrays(1, &fullscreenVAO);
    glGenBuffers(1, &fullscreenVBO);
    glBindVertexArray(fullscreenVAO);
    glBindBuffer(GL_ARRAY_BUFFER, fullscreenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fullscreenVerts),
        &fullscreenVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(f32), (void*)0);
    glBindVertexArray(0);

    // Generate SSAO kernel
    std::uniform_real_distribution<float> randomFloats(0.05, 1.0); // random floats between 0.0 - 1.0
    std::default_random_engine generator;
    for (unsigned int i = 0; i < Kernel_Size; ++i)
    {
        vec3 sample(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator)
        );
        sample.normalize();
        sample *= randomFloats(generator);
        float scale = (float)i / (float)Kernel_Size;
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel[i] = sample;
    }

    // create SSAO noise texture
    std::vector<vec3> ssaoNoise;
    for (int i = 0; i < 16; i++) {
        vec3 noise(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            0.0f
        );
        ssaoNoise.push_back(noise);
    }
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Create SSAO FBO
    ssaoFBO.addColorBufferObject("ssao", 0, GL_RED, GL_RED, GL_UNSIGNED_BYTE);
    ssaoFBO.create(ResolutionScale::OneHalf);
    ssaoBlurFBO.addColorBufferObject("ssao", 0, GL_RED, GL_RED, GL_UNSIGNED_BYTE);
    ssaoBlurFBO.create(ResolutionScale::OneHalf);

    return this;
}
*/

void DeferredBatchRenderer::renderBatch(RenderBatch* batch) {
    /* Use list of draw calls to render the scene in multiple passes
    1. Build geometry buffer
    */

    beginProfile();

    if (false /*GetCurrentScene()*/) {

        geometryPass(batch);
        dur_geometryPass = profileRenderPass();

        ssaoPass(batch);
        dur_ssaoPass = profileRenderPass();

        screenPass(batch);
        dur_screenPass = profileRenderPass();
    }

    endProfile();
}

void DeferredBatchRenderer::geometryPass(RenderBatch* batch) {
    // bind geometry buffer
    m_gBuffer.bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_BLEND);
    m_geometryPassShader.use();
    m_geometryPassShader.setMat4("projectionMatrix", 
        batch->cameraProjection);

    m_geometryPassShader.setInt("material.baseColorTexture", 0);
    m_geometryPassShader.setInt("material.normalTexture", 1);
    m_geometryPassShader.setInt("material.metallicRoughnessTexture", 2);
    m_geometryPassShader.setInt("material.occlusionTexture", 3);
    m_geometryPassShader.setInt("material.emissiveTexture", 4);

    for (int n = 0; n < batch->numCalls; n++) {
        DrawCall* call = &batch->calls[n];

        // Render entity
        glBindVertexArray(call->VAO);

        if (call->mat) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, call->mat->baseColorTexture.glTexID == 0 ? whiteTex.glTextureID : call->mat->baseColorTexture.glTexID);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, call->mat->normalTexture.glTexID == 0 ? normalTex.glTextureID : call->mat->normalTexture.glTexID);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, call->mat->amrTexture.glTexID == 0 ? whiteTex.glTextureID : call->mat->amrTexture.glTexID);

            //glActiveTexture(GL_TEXTURE3);
            //glBindTexture(GL_TEXTURE_2D, call->mat->occlusionTexture.glTexID == 0 ? whiteTex.glTextureID : call->mat->occlusionTexture.glTexID);

            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, call->mat->emissiveTexture.glTexID == 0 ? whiteTex.glTextureID : call->mat->emissiveTexture.glTexID);

            m_geometryPassShader.setVec3("material.emissiveFactor", call->mat->emissiveFactor);
            m_geometryPassShader.setVec4("material.baseColorFactor", call->mat->baseColorFactor);
            m_geometryPassShader.setFloat("material.metallicFactor", call->mat->metallicFactor);
            m_geometryPassShader.setFloat("material.roughnessFactor", call->mat->roughnessFactor);

            m_geometryPassShader.setMat4("modelViewMatrix", batch->cameraView * call->modelMatrix);
        }

        glDrawElements(GL_TRIANGLES, call->numVerts, GL_UNSIGNED_SHORT, 0);
    }

    // unbind gBuffer
    m_gBuffer.unbind();
    glEnable(GL_BLEND);
}

void DeferredBatchRenderer::ssaoPass(RenderBatch* batch) {
    ssaoFBO.bind();

    glClear(GL_COLOR_BUFFER_BIT);

    m_ssaoPassShader.use();

    m_ssaoPassShader.setInt("TargetPos", 0);
    m_ssaoPassShader.setInt("Target1", 1); 
    m_ssaoPassShader.setInt("texNoise", 2);
    m_ssaoPassShader.setMat4("projectionMatrix", batch->cameraProjection);
    m_ssaoPassShader.setVec2("noiseScale", ssaoFBO.getRenderSize()/4.0);

    for (int n = 0; n < Kernel_Size; n++) {
        m_ssaoPassShader.setVec3("Kernel[" + std::to_string(n) + "]", ssaoKernel[n]);
    }

    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, m_gBuffer.getColorBuffer("TargetPos"));
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, m_gBuffer.getColorBuffer("Target1"));
    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, this->noiseTexture);

    glBindVertexArray(fullscreenVAO);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBindVertexArray(0);

    ssaoFBO.unbind();

    // Blur the SSAO values
    ssaoBlurFBO.bind();

    glClear(GL_COLOR_BUFFER_BIT);

    m_ssaoBlurShader.use();

    m_ssaoBlurShader.setInt("ssaoInput", 0);
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, ssaoFBO.getColorBuffer("ssao"));

    glBindVertexArray(fullscreenVAO);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBindVertexArray(0);

    ssaoBlurFBO.unbind();
}

void DeferredBatchRenderer::screenPass(RenderBatch* batch) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_screenShader.use();

    m_screenShader.setInt("shaderOutput", shaderOutput);

    m_screenShader.setInt("Target0", 0);
    m_screenShader.setInt("Target1", 1);
    m_screenShader.setInt("Target2", 2);
    m_screenShader.setInt("TargetDepth", 3);
    m_screenShader.setInt("TargetPos", 4);
    m_screenShader.setInt("SSAO", 5);

    glActiveTexture(GL_TEXTURE0);glBindTexture(GL_TEXTURE_2D, m_gBuffer.getColorBuffer("Target0")); //m_gBuffer.rt0);
    glActiveTexture(GL_TEXTURE1);glBindTexture(GL_TEXTURE_2D, m_gBuffer.getColorBuffer("Target1")); //m_gBuffer.rt1);
    glActiveTexture(GL_TEXTURE2);glBindTexture(GL_TEXTURE_2D, m_gBuffer.getColorBuffer("Target2")); //m_gBuffer.rt2);
    glActiveTexture(GL_TEXTURE3);glBindTexture(GL_TEXTURE_2D, m_gBuffer.getColorBuffer("TargetDepth")); //m_gBuffer.rtDepth);
    glActiveTexture(GL_TEXTURE4);glBindTexture(GL_TEXTURE_2D, m_gBuffer.getColorBuffer("TargetPos")); //m_gBuffer.rtPos);
    glActiveTexture(GL_TEXTURE5);glBindTexture(GL_TEXTURE_2D, ssaoBlurFBO.getColorBuffer("ssao"));

    glBindVertexArray(fullscreenVAO);

    glDisable(GL_CULL_FACE);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_CULL_FACE);

    glBindVertexArray(0);
}



void DeferredBatchRenderer::renderDebug(
    RenderBatch* batch,
    double frameCount,
    long long lastFrame,
    bool debugMode) {

    profileStart = _clock::now();

    vec4 white(1, 1, 1, 1);
    char text[128];
    long long scale = 1LL;
    int y = -13;

    sprintf(text, "FPS: %-2.1lf [%lld us]", 1000000.0 / static_cast<double>(frameCount), lastFrame);
    debugFont.drawText(scr_width - 5, 5, white, text, ALIGN_TOP_RIGHT);
    //sprintf(text, "FPS Lock: %s",
    //    g_options.limitFramerate ? (g_options.highFramerate ? "250" : "50") : "NONE");
    debugFont.drawText(scr_width - 5, 23, white, text, ALIGN_TOP_RIGHT);

    sprintf(text, "Render......%-3lld us (%-3lld us avg.)",
        dur_fullRenderPass / scale,
        static_cast<long long>(avgRenderPass.getCurrentAverage()) / scale);
    debugFont.drawText(5, y += 18, white, text, ALIGN_TOP_LEFT);

    if (false /*GetCurrentScene()*/ && (debugMode)) {
        sprintf(text, " Geometry Pass.......%-3lld us", dur_geometryPass / scale);
        debugFont.drawText(5, y += 18, white, text, ALIGN_TOP_LEFT);

        sprintf(text, " SSAO Pass...........%-3lld us", dur_ssaoPass / scale);
        debugFont.drawText(5, y += 18, white, text, ALIGN_TOP_LEFT);

        sprintf(text, " Screen Pass.........%-3lld us", dur_screenPass / scale);
        debugFont.drawText(5, y += 18, white, text, ALIGN_TOP_LEFT);

        sprintf(text, "Draw Calls: %-3d", batch->numCalls);
        debugFont.drawText(5, scr_height - 5, white, text, ALIGN_BOT_LEFT);

        sprintf(text, "Shader Output: %-3d [%s]", shaderOutput, soStr.c_str());
        debugFont.drawText(5, scr_height - 25, white, text, ALIGN_BOT_LEFT);

        glClear(GL_DEPTH_BUFFER_BIT);
        m_debugMeshShader.use();
        m_debugMeshShader.setMat4("projectionViewMatrix", batch->cameraProjection * batch->cameraView);
        m_debugMeshShader.setMat4("modelMatrix", batch->cameraModelMatrix);
        m_debugMeshShader.setVec3("sun.direction", batch->sun->Direction);
        m_debugMeshShader.setVec3("sun.color", batch->sun->Color);
        m_debugMeshShader.setFloat("sun.strength", batch->sun->Strength);
        m_debugMeshShader.setVec3("objColor", vec3(1, 1, 1));
        m_debugMeshShader.setVec3("camPos", batch->camPos); // use viewPos so camera model has correct specular

        glBindVertexArray(cameraMesh->VAO);
        glDrawElements(GL_TRIANGLES, cameraMesh->numFaces * 3, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
    }
    sprintf(text, "Debug.......%-3lld us", dur_debug / scale);
    debugFont.drawText(5, y += 18, white, text, ALIGN_TOP_LEFT);

    // Print gamepad state
    if (Input::gamepadPresent) {
        int numAxes;
        const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &numAxes);

        for (int jj = 0; jj < numAxes; jj++) {
            sprintf(text, "Gamepad axis %d: %.3f", jj, axes[jj]);
            debugFont.drawText(5, 100 + jj * 20, white, text);
        }

        int numButtons;
        const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &numButtons);

        for (int jj = 0; jj < numButtons; jj++) {
            sprintf(text, "Gamepad button %d: %s", jj, buttons[jj] == GLFW_PRESS ? "Pressed" : "");
            debugFont.drawText(5, 100 + numAxes * 20 + jj * 20, white, text);
        }
    }

    dur_debug = profileRenderPass();
}






/* Render Profiling */
void DeferredBatchRenderer::beginProfile() {
    dur_fullRenderPass = 0;
    profileStart = _clock::now();
}
DeferredBatchRenderer::_dur DeferredBatchRenderer::profileRenderPass() {
    _dur duration =
        std::chrono::duration_cast<std::chrono::microseconds>(
            _clock::now() - profileStart).count();

    dur_fullRenderPass += duration;

    profileStart = _clock::now();
    return duration;
}
void DeferredBatchRenderer::endProfile() {
    avgRenderPass.addSample(dur_fullRenderPass);
}

void DeferredBatchRenderer::loadResources() {
    /* FIX_THIS */
    //resource->loadModelFromFile("Data/Models/camera.glb", true);

    cameraMesh = ResourceCatalog::GetInstance()->getMesh("Camera");
}
