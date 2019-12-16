#include "BatchRenderer.hpp"
#include "Scene/Scene.hpp"
#include "Collision/Visualizer.hpp"

Visualizer viz;

BatchRenderer::BatchRenderer() : colliderID(1)
{}

BatchRenderer::~BatchRenderer() {}

void BatchRenderer::update(double dt) {
}

void BatchRenderer::handleMessage(Message msg) {
    if (msg.isType("InputKey")) {
        // int button, int action, int mods
        using dt = Message::Datatype;
        dt key = msg.data[0];
        dt scancode = msg.data[1];
        dt action = msg.data[2];
        dt mods = msg.data[3];

        if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {
            Console::logMessage("Reloading Shaders");

            m_shadowPass.create("shadow.vert", "shadow.frag", "shadowPassShader");
            m_staticPass.create("static_pbr_withShadow.vert", "static_pbr_withShadow.frag", "staticPassShader");
            m_skyboxPass.create("skybox.vert", "skybox.frag", "skyboxPassShader");
            m_lightVolumePass.create("lightVolume.vert", "lightVolume.frag", "lightVolumeShader");
            m_toneMap.create("toneMap.vert", "toneMap.frag", "toneMapShader");
            m_gammaCorrect.create("toneMap.vert", "gammaCorrect.frag", "gammaCorrectShader");
            m_debugLineShader.create("DebugLine.vert", "DebugLine.frag", "debugLineShader");
            m_debugMeshShader.create("debugMesh.vert", "debugMesh.frag", "debugMeshShader");
            m_pickPassShader.create("shadow.vert", "pickPass.frag", "pickPassShader");
            //m_wireframeShader.create("wireframe.vert", "wireframe.frag", "wireframeShader");
        }

        if (key == GLFW_KEY_V && action == GLFW_PRESS) {
            colliderID++;

            if (colliderID > cWorld.m_static.size())
                colliderID = 1;
        }
    }

    if (msg.isType("NewWindowSize")) {
        using dt = Message::Datatype;
        dt newX = msg.data[0];
        dt newY = msg.data[1];

        fb.resize(newX, newY);
        fb_volume.resize(newX, newY);
        fb_toneMap.resize(newX, newY);

        debugFont.resize(newX, newY);
        debugFontSmall.resize(newX, newY);

        scr_width = newX;
        scr_height = newY;
    }
}

UID_t BatchRenderer::pickObject(GLint x, GLint y) {
    fb_pick.bind();

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    auto i = static_cast<GLint>(x / 4);
    auto j = static_cast<GLint>((scr_height - y) / 4);

    u8 pixels[4] = { 0 };
    glReadPixels(i, j, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    int id = pixels[0] + pixels[1] * 256 + pixels[2] * 256 * 256;

    return id;
}

void BatchRenderer::destroy() {}
CoreSystem* BatchRenderer::create() {
    m_shadowPass.create("shadow.vert", "shadow.frag", "shadowPassShader");
    m_staticPass.create("static_pbr_withShadow.vert", "static_pbr_withShadow.frag", "staticPassShader");
    m_skyboxPass.create("skybox.vert", "skybox.frag", "skyboxPassShader");
    m_lightVolumePass.create("lightVolume.vert", "lightVolume.frag", "lightVolumeShader");
    m_toneMap.create("toneMap.vert", "toneMap.frag", "toneMapShader");
    m_gammaCorrect.create("toneMap.vert", "gammaCorrect.frag", "gammaCorrectShader");
    m_debugLineShader.create("DebugLine.vert", "DebugLine.frag", "debugLineShader");
    m_debugMeshShader.create("debugMesh.vert", "debugMesh.frag", "debugMeshShader");
    m_pickPassShader.create("shadow.vert", "pickPass.frag", "pickPassShader");
    //m_wireframeShader.create("wireframe.vert", "wireframe.frag", "wireframeShader");

    blackTex.loadImage("black.png");
    whiteTex.loadImage("white.png");
    normalTex.loadImage("normal.png");
    greenTex.loadImage("green.png");

    Shadowmap::initShadows();
    sm.create(2048, 2048); // resolution of shadow map

    scr_width = DEFAULT_SCREEN_WIDTH;
    scr_height = DEFAULT_SCREEN_HEIGHT;

    fb.create(scr_width, scr_height);
    fb_volume.create(scr_width, scr_height);
    fb_toneMap.create(scr_width, scr_height);
    fb_pick.create_LDR(scr_width/4, scr_height/4);

    debugFont.InitTextRendering();
    debugFont.create("UbuntuMono-Regular.ttf", 16, scr_width, scr_height);
    debugFontSmall.InitTextRendering();
    debugFontSmall.create("UbuntuMono-Regular.ttf", 10, scr_width, scr_height);

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


    const f32 debugLineVerts[] = {
        0, 
        1
    };

    GLuint debugLineVBO;
    glGenVertexArrays(1, &debugLineVAO);
    glGenBuffers(1, &debugLineVBO);
    glBindVertexArray(debugLineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, debugLineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(debugLineVerts),
        &debugLineVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(f32), (void*)0);

    glBindVertexArray(0);

    return this;
}

void BatchRenderer::renderBatch(RenderBatch* batch) {
    /* Use list of draw calls to render the scene in multiple passes
    1. Shadow Pass
    2. Static Entity Pass
    3. Dynamic Entity Pass
    4. Skybox Pass
    5. Volumetric Light Pass
    6. HDR Tone Map
    7. Gamma Correction
    */

    beginProfile();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (GetScene()) {

        pickPass(batch);
        dur_pickPass = profileRenderPass();

        shadowPass(batch);
        dur_shadowPass = profileRenderPass();

        fb.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //static const float black[] = { 0, 0, 0, 1 };
        //static const float red[] = { -100, 0, -100, 1 };
        //glClearBufferfv(GL_COLOR, 0, black);
        //glClearBufferfv(GL_COLOR, 1, red);

        staticPass(batch);
        dur_staticPass = profileRenderPass();

        dynamicPass(batch);
        dur_dynamicPass = profileRenderPass();

        skyboxPass(batch);
        dur_skyboxPass = profileRenderPass();

        fb.unbind();
        fb_volume.bind();
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightVolumePass(batch);
        dur_lightVolumePass = profileRenderPass();

        fb_volume.unbind();
        fb_toneMap.bind();
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        toneMap(batch);
        dur_toneMap = profileRenderPass();

        fb_toneMap.unbind();

        gammaCorrect(batch);
        dur_gammaCorrect = profileRenderPass();
    }

    endProfile();
}

void BatchRenderer::shadowPass(RenderBatch* batch) {
    // First, render to depth map
    m_shadowPass.use();
    glViewport(0, 0, sm.width, sm.height);
    glBindFramebuffer(GL_FRAMEBUFFER, sm.depthMapFBO);
    glClearColor(1, 1, 1, 1);
    glClear(GL_DEPTH_BUFFER_BIT);

    m_shadowPass.setMat4("projectionViewMatrix", batch->sunViewProjectionMatrix);
    for (int n = 0; n < batch->numCalls; n++) {
        DrawCall* draw = &batch->calls[n];

        glBindVertexArray(draw->VAO);
        m_shadowPass.setMat4("modelMatrix", draw->modelMatrix);
        glDrawElements(GL_TRIANGLES, draw->numVerts, GL_UNSIGNED_SHORT, 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, scr_width, scr_height);
}

void BatchRenderer::pickPass(RenderBatch* batch) {
    // bind pickPass framebuffer
    // bind viewProjection matrix
    // For every entity in view:
        //bind the entities model transform
        //bind the entities obb transform
        //determine unique entity color
        //bind cube VAO and render
        // First, render to depth map
    m_pickPassShader.use();
    fb_pick.bind();
    glViewport(0, 0, scr_width/4, scr_height/4); // downsample so its faster i guess
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    m_pickPassShader.setMat4("projectionViewMatrix", batch->cameraViewProjectionMatrix);
    for (int n = 0; n < batch->numCalls; n++) {
        DrawCall* draw = &batch->calls[n];
        glBindVertexArray(draw->VAO); // draw the base mesh for now, might use a simpler mesh later
        m_pickPassShader.setMat4("modelMatrix", draw->modelMatrix);
        m_pickPassShader.setVec3("idColor", getUniqueColor(draw->id));
        glDrawElements(GL_TRIANGLES, draw->numVerts, GL_UNSIGNED_SHORT, 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, scr_width, scr_height);
}

vec3 BatchRenderer::getUniqueColor(UID_t id) {
    int r = (id & 0x000000FF) >> 0;
    int g = (id & 0x0000FF00) >> 8;
    int b = (id & 0x00FF0000) >> 16;

    return vec3(r/255.0f, g / 255.0f, b / 255.0f);
}

void BatchRenderer::staticPass(RenderBatch* batch) {
    glEnable(GL_DEPTH_TEST);

    m_staticPass.use();
    m_staticPass.setLights(
        batch->sun,
        batch->pointLights,
        batch->spotLights
    );

    m_staticPass.setMat4("projectionViewMatrix", batch->cameraViewProjectionMatrix);
    m_staticPass.setVec3("camPos", batch->camPos);

    m_staticPass.setInt("material.baseColorTexture", 0);
    m_staticPass.setInt("material.normalTexture", 1);
    m_staticPass.setInt("material.metallicRoughnessTexture", 2);
    m_staticPass.setInt("material.occlusionTexture", 3);
    m_staticPass.setInt("material.emissiveTexture", 4);
    m_staticPass.setInt("irradianceMap", 5);
    m_staticPass.setInt("prefilterMap", 6);
    m_staticPass.setInt("brdfLUT", 7);
    m_staticPass.setInt("shadowMap", 8);

    m_staticPass.setMat4("lightProjectionViewMatrix", batch->sunViewProjectionMatrix);

    batch->env->bindPBR(GL_TEXTURE5, GL_TEXTURE6, GL_TEXTURE7);
    sm.bind(GL_TEXTURE8);

    /* Render Entities */
    for (int n = 0; n < batch->numCalls; n++) {
        DrawCall* call = &batch->calls[n];
        // Render entity ent
        glBindVertexArray(call->VAO);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, call->mat->baseColorTexture.glTexID == 0 ? whiteTex.glTextureID : call->mat->baseColorTexture.glTexID);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, call->mat->normalTexture.glTexID == 0 ? normalTex.glTextureID : call->mat->normalTexture.glTexID);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, call->mat->metallicRoughnessTexture.glTexID == 0 ? whiteTex.glTextureID : call->mat->metallicRoughnessTexture.glTexID);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, call->mat->occlusionTexture.glTexID == 0 ? whiteTex.glTextureID : call->mat->occlusionTexture.glTexID);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, call->mat->emissiveTexture.glTexID == 0 ? whiteTex.glTextureID : call->mat->emissiveTexture.glTexID);

        m_staticPass.setVec3("material.emissiveFactor", call->mat->emissiveFactor);
        m_staticPass.setVec4("material.baseColorFactor", call->mat->baseColorFactor);
        m_staticPass.setFloat("material.metallicFactor", call->mat->metallicFactor);
        m_staticPass.setFloat("material.roughnessFactor", call->mat->roughnessFactor);

        m_staticPass.setMat4("modelMatrix", call->modelMatrix);
        
        glDrawElements(GL_TRIANGLES, call->numVerts, GL_UNSIGNED_SHORT, 0);
    }
}

void BatchRenderer::dynamicPass(RenderBatch* batch) {
}

void BatchRenderer::skyboxPass(RenderBatch* batch) {
    m_skyboxPass.use();
    mat4 identity;
    m_skyboxPass.setMat4("viewMatrix", mat4(mat3(batch->cameraView)));
    m_skyboxPass.setMat4("projectionMatrix", batch->cameraProjection);

    m_skyboxPass.setInt("skybox", 0);
    //skybox->bind(GL_TEXTURE0);
    batch->env->bindSkybox(GL_TEXTURE0);

    glDepthFunc(GL_LEQUAL);
    glBindVertexArray(batch->env->skyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);
}

void BatchRenderer::lightVolumePass(RenderBatch* batch) {
    m_lightVolumePass.use();
    m_lightVolumePass.setInt("tex", 0);
    m_lightVolumePass.setInt("positionMap", 1);
    m_lightVolumePass.setInt("shadowMap", 2);
    m_lightVolumePass.setVec3("camPos", batch->camPos);
    m_lightVolumePass.setVec3("sun.direction", batch->sun->Direction);
    m_lightVolumePass.setVec3("sun.color", batch->sun->Color);
    m_lightVolumePass.setFloat("sun.strength", batch->sun->Strength);
    m_lightVolumePass.setMat4("lightProjectionViewMatrix", batch->sunViewProjectionMatrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fb.getTexture());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, fb.getPositionTex());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, sm.depthMap);

    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(fullscreenVAO);
    glDisable(GL_CULL_FACE);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

void BatchRenderer::toneMap(RenderBatch* batch) {
    // read from FBO, render to screen buffer
    // perform hdr correction
    m_toneMap.use();
    m_toneMap.setInt("tex", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fb_volume.getTexture());

    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(fullscreenVAO);
    glDisable(GL_CULL_FACE);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

void BatchRenderer::gammaCorrect(RenderBatch* batch) {
    // read from FBO, render to screen buffer
    // perform hdr correction
    m_gammaCorrect.use();
    m_gammaCorrect.setInt("tex", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fb_toneMap.getTexture());

    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(fullscreenVAO);
    glDisable(GL_CULL_FACE);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

void BatchRenderer::renderDebug(
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
    sprintf(text, "FPS Lock: %s",
        g_options.limitFramerate ? (g_options.highFramerate ? "250" : "50") : "NONE");
    debugFont.drawText(scr_width - 5, 23, white, text, ALIGN_TOP_RIGHT);

    sprintf(text, "Render - - - %-3lld us (%-3lld us avg.)",
        dur_fullRenderPass / scale,
        static_cast<long long>(avgRenderPass.getCurrentAverage()) / scale);
    debugFont.drawText(5, y += 18, white, text, ALIGN_TOP_LEFT);

    sprintf(text, "[T] Relative Movement: %s", static_cast<CharacterObject*>(GetScene()->getObjectByName("YaBoy"))->GetRelativeMovementType());
    debugFont.drawText(5, scr_height - 18, white, text, ALIGN_BOT_LEFT);
    sprintf(text, "[Y] Control Type: %s", static_cast<PlayerObject*>(GetScene()->getObjectByName("YaBoy"))->GetControlType());
    debugFont.drawText(5, scr_height - 31, white, text, ALIGN_BOT_LEFT);

    sprintf(text, "MoveForward: %.2f", Input::getAxisState("MoveForward"));
    debugFont.drawText(scr_width - 5, scr_height - 5, white, text, ALIGN_BOT_RIGHT);
    sprintf(text, "MoveRight: %.2f", Input::getAxisState("MoveRight"));
    debugFont.drawText(scr_width - 5, scr_height - 18, white, text, ALIGN_BOT_RIGHT);

    if (GetScene() && (false || debugMode)) {
        sprintf(text, " Shadow- - - %-3lld us", dur_shadowPass / scale);
        debugFont.drawText(5, y += 18, white, text, ALIGN_TOP_LEFT);
        sprintf(text, " Static- - - %-3lld us", dur_staticPass / scale);
        debugFont.drawText(5, y += 18, white, text, ALIGN_TOP_LEFT);
        sprintf(text, " Dynamic - - %-3lld us", dur_dynamicPass / scale);
        debugFont.drawText(5, y += 18, white, text, ALIGN_TOP_LEFT);
        sprintf(text, " Skybox- - - %-3lld us", dur_skyboxPass / scale);
        debugFont.drawText(5, y += 18, white, text, ALIGN_TOP_LEFT);
        sprintf(text, " Volume- - - %-3lld us", dur_lightVolumePass / scale);
        debugFont.drawText(5, y += 18, white, text, ALIGN_TOP_LEFT);
        sprintf(text, " ToneMap - - %-3lld us", dur_toneMap / scale);
        debugFont.drawText(5, y += 18, white, text, ALIGN_TOP_LEFT);
        sprintf(text, " Gamma - - - %-3lld us", dur_gammaCorrect / scale);
        debugFont.drawText(5, y += 18, white, text, ALIGN_TOP_LEFT);
        sprintf(text, " Pick- - - - %-3lld us", dur_pickPass / scale);
        debugFont.drawText(5, y += 18, white, text, ALIGN_TOP_LEFT);

        sprintf(text, "Draw Calls: %-3d", batch->numCalls);
        debugFont.drawText(5, scr_height - 5, white, text, ALIGN_BOT_LEFT);

        glClear(GL_DEPTH_BUFFER_BIT);
        m_debugMeshShader.use();
        m_debugMeshShader.setMat4("projectionViewMatrix", batch->cameraViewProjectionMatrix);
        m_debugMeshShader.setMat4("modelMatrix", batch->cameraModelMatrix);
        m_debugMeshShader.setVec3("sun.direction", batch->sun->Direction);
        m_debugMeshShader.setVec3("sun.color", batch->sun->Color);
        m_debugMeshShader.setFloat("sun.strength", batch->sun->Strength);
        m_debugMeshShader.setVec3("objColor", vec3(1, 1, 1));
        m_debugMeshShader.setVec3("camPos", batch->viewPos); // use viewPos so camera model has correct specular

        glBindVertexArray(cameraMesh->VAO);
        if (debugMode)
            glDrawElements(GL_TRIANGLES, cameraMesh->numFaces * 3, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);

        // Draw wireframe things
        glDisable(GL_DEPTH_TEST);
        m_debugLineShader.use();
        m_debugLineShader.setMat4("projectionViewMatrix", batch->cameraViewProjectionMatrix);

        for (auto k : GetScene()->m_masterMap) {
            auto go = k.second;
            vec3 vPos = go->Position;
            mat3 meshTransform = go->getTransform();
            vec3 vFor = vec3(meshTransform.col1());
            vec3 vUp = vec3(meshTransform.col2());
            vec3 vRight = vec3(meshTransform.col3());

            glDisable(GL_DEPTH_TEST);
            m_debugLineShader.use();

            drawLine(vPos, vPos + vFor, vec3(1, 0, 0), vec3(1, 0, 0));
            drawLine(vPos, vPos + vUp, vec3(0, 1, 0), vec3(0, 1, 0));
            drawLine(vPos, vPos + vRight, vec3(0, 0, 1), vec3(0, 0, 1));

            vec4 _screenPos = batch->cameraViewProjectionMatrix * vec4(vPos, 1);
            if (_screenPos.w > 0) {
                vec2 screenPos = (vec2(_screenPos.x / _screenPos.w, _screenPos.y / _screenPos.w) / 2) + vec2(.5);
                screenPos = vec2(screenPos.x, 1 - screenPos.y);

                sprintf(text, "%s:%llu{%s}", go->ObjectTypeString(), go->getID(), go->Name.c_str());
                debugFontSmall.drawText(screenPos.x * scr_width, screenPos.y * scr_height, white, text, ALIGN_TOP_LEFT);
            }
        }

        glDisable(GL_DEPTH_TEST);
        m_debugLineShader.use();
        for (auto k : GetScene()->objectsByType.Volumes) {
            vec3 pos = k->Position;
            drawAABBB(pos, k->bounds_min, k->bounds_max, k->Inside() ? vec3(1, .2, .2) : vec3(1));
        }
        for (auto k : GetScene()->objectsByType.DirLights) {
            vec3 pos = k->Position;
            vec3 dir = k->Direction;
            drawLine(pos, pos + dir * 10, k->Color, k->Color);
        }
        for (auto k : GetScene()->objectsByType.SpotLights) {
            vec3 pos = k->Position;
            vec3 dir = k->Direction;
            drawLine(pos, pos + dir * 3, k->Color, k->Color);
        }
        for (auto k : GetScene()->objectsByType.PointLights) {
            vec3 pos = k->Position;
            const scalar pointLightLength = .3;
            drawLine(pos, pos + vec3(1, 1, 1) *pointLightLength, k->Color, k->Color);
            drawLine(pos, pos + vec3(1, 1, -1) *pointLightLength, k->Color, k->Color);
            drawLine(pos, pos + vec3(-1, 1, 1) *pointLightLength, k->Color, k->Color);
            drawLine(pos, pos + vec3(-1, 1, -1) *pointLightLength, k->Color, k->Color);

            drawLine(pos, pos + vec3(1, -1, 1) *pointLightLength, k->Color, k->Color);
            drawLine(pos, pos + vec3(1, -1, -1) *pointLightLength, k->Color, k->Color);
            drawLine(pos, pos + vec3(-1, -1, 1) *pointLightLength, k->Color, k->Color);
            drawLine(pos, pos + vec3(-1, -1, -1) *pointLightLength, k->Color, k->Color);
        }

        glBindVertexArray(0);

        // Draw buffered wireframe meshes
        /*m_pickPassShader.use();
        m_pickPassShader.setMat4("projectionViewMatrix", batch->cameraViewProjectionMatrix);
        m_pickPassShader.setVec3("idColor", vec3(1, .2, .4));
        for (auto k : GetScene()->objectsByType.Collisions) {
            auto hull = &k->m_hull;

            m_pickPassShader.setMat4("modelMatrix", k->getTransform());

            glBindVertexArray(hull->wireframeVAO);
            glDrawElements(GL_LINES, hull->edges.m_numElements * 2, GL_UNSIGNED_SHORT, 0);
        }*/

        glEnable(GL_DEPTH_TEST);
    }
    sprintf(text, "Debug- - - - %-3lld us", dur_debug / scale);
    debugFont.drawText(5, y += 18, white, text, ALIGN_TOP_LEFT);

    ///////////////////////////////////////////////////////////////////////////////////////
    // TEMPORARY VISUALIZATION
    vec3 red = vec3(1, 0, 0);
    vec3 orange = vec3(1, .5, .3);
    vec3 green = vec3(.2, .5, .8);

    m_debugLineShader.use();
    m_debugLineShader.setMat4("projectionViewMatrix", batch->cameraViewProjectionMatrix);
    for (int n = 0; n < viz.m_lines.size(); n++) {
        drawLine(viz.m_lines[n].a, viz.m_lines[n].b, orange, orange);
        drawPoint(viz.m_lines[n].a, red);
        drawPoint(viz.m_lines[n].b, red);
    }
    drawLine(viz.res.start, viz.res.end, orange, red);
    drawPoint(viz.res.contactPoint, green);

    auto character = GetScene()->objectsByType.Players[0];// ->getHull();
    //drawLine(character->res.start, character->res.end, orange, red);
    //drawPoint(character->res.contactPoint, green);

    /* Draw GJK result from player to crate */
    auto crateHull = cWorld.getHullFromID(colliderID);
    auto playerHull = character->getHull();
    gjk_Input in;
    in.hull1 = playerHull;
    in.hull2 = crateHull;
    gjk_Output out;
    cWorld.GJK(&out, in);
    drawLine(out.point1, out.point2, red, red);

    sprintf(text, "Distance from [%d]: %.4f", colliderID, out.distance);
    debugFont.drawText(300, 0, vec4(orange,1), text);
    sprintf(text, "Grounded: %s", 
        character->grounded ? "Yes" : "No");
    debugFont.drawText(300, 20, vec4(orange, 1), text);
    sprintf(text, "Floor up: (%.4f,%.4f,%.4f)", 
        character->m_floorUp.x,
        character->m_floorUp.y,
        character->m_floorUp.z);
    debugFont.drawText(300, 40, vec4(orange, 1), text);
    sprintf(text, "Velocity: (%.4f,%.4f,%.4f)",
        character->Velocity.x,
        character->Velocity.y,
        character->Velocity.z);
    debugFont.drawText(300, 60, vec4(orange, 1), text);
    int nonzero = 0;
    for (int n = 0; n < character->res.numContacts; n++) {
        if (character->res.planes[n].TOI < .02) {
            nonzero++;
            sprintf(text, " ToI: %.4f: (%.1f,%.1f,%.1f)", 
                character->res.planes[n].TOI,
                character->res.planes[n].contact_point.x,
                character->res.planes[n].contact_point.y,
                character->res.planes[n].contact_point.z);
            debugFont.drawText(300, 100 + 20 * n, vec4(orange, 1), text);
        }
    }
    sprintf(text, "Contact planes: %d, %d iterations", nonzero, character->iterations);
    debugFont.drawText(300, 80, vec4(orange, 1), text);

    // Draw buffered wireframe meshes
    m_pickPassShader.use();
    m_pickPassShader.setMat4("projectionViewMatrix", batch->cameraViewProjectionMatrix);
    m_pickPassShader.setVec3("idColor", vec3(.7, 0, .6));
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glDisable(GL_CULL_FACE);

    m_pickPassShader.setVec3("idColor", vec3(.7, .4, .6));
    m_pickPassShader.setMat4("modelMatrix", mat4(
        vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), 
        vec4(0, 0, 1, 0), vec4(character->ghostPosition + vec3(0,1,0), 1)));
    glBindVertexArray(character->getMesh()->VAO);
    glDrawElements(GL_TRIANGLES, character->getMesh()->numFaces * 3, GL_UNSIGNED_SHORT, 0);

    m_pickPassShader.setVec3("idColor", vec3(.7, 0, .6));
    // Render Collision World
    for (int n = 1; n < cWorld.m_static.size(); n++) {
        auto cHull = &cWorld.m_static[n];
        m_pickPassShader.setMat4("modelMatrix", mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), vec4(cHull->position, 1)) * mat4(cHull->rotation));
        glBindVertexArray(cHull->wireframeVAO);
        glDrawElements(GL_TRIANGLES, cHull->faces.m_numElements * 3, GL_UNSIGNED_SHORT, 0);
    }
    m_pickPassShader.setVec3("idColor", vec3(.2, .5, .8));
    for (int n = 0; n < cWorld.m_dynamic.size(); n++) {
        auto cHull = &cWorld.m_dynamic[n];
        m_pickPassShader.setMat4("modelMatrix", mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), vec4(cHull->position, 1)) * mat4(cHull->rotation));
        glBindVertexArray(cHull->wireframeVAO);
        glDrawElements(GL_TRIANGLES, cHull->faces.m_numElements * 3, GL_UNSIGNED_SHORT, 0);
    }
    //glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    ///////////////////////////////////////////////////////////////////////////////////////

    dur_debug = profileRenderPass();
}

void BatchRenderer::drawLine(vec3 A, vec3 B, vec3 colorA, vec3 colorB) {
    m_debugLineShader.setVec3("vertexA", A);
    m_debugLineShader.setVec3("vertexB", B);
    m_debugLineShader.setVec3("colorA", colorA);
    m_debugLineShader.setVec3("colorB", colorB);

    glBindVertexArray(debugLineVAO);
    glDrawArrays(GL_LINES, 0, 2);
}

void BatchRenderer::drawPoint(vec3 P, vec3 Color) {
    m_debugLineShader.setVec3("vertexA", P);
    m_debugLineShader.setVec3("vertexB", P);
    m_debugLineShader.setVec3("colorA", Color);
    m_debugLineShader.setVec3("colorB", Color);

    glBindVertexArray(debugLineVAO);
    glDrawArrays(GL_POINTS, 0, 1);
}

void BatchRenderer::drawAABBB(vec3 center, vec3 min, vec3 max, vec3 color) {
    vec3 a = center + min;
    vec3 b = center + max;

    vec3 v0 = vec3(a.x, a.y, a.z);
    vec3 v1 = vec3(a.x, a.y, b.z);
    vec3 v2 = vec3(b.x, a.y, b.z);
    vec3 v3 = vec3(b.x, a.y, a.z);

    vec3 v4 = vec3(a.x, b.y, a.z);
    vec3 v5 = vec3(a.x, b.y, b.z);
    vec3 v6 = vec3(b.x, b.y, b.z);
    vec3 v7 = vec3(b.x, b.y, a.z);

    // Bottom
    drawLine(v0, v1, color, color);
    drawLine(v1, v2, color, color);
    drawLine(v2, v3, color, color);
    drawLine(v3, v0, color, color);

    // Top
    drawLine(v4, v5, color, color);
    drawLine(v5, v6, color, color);
    drawLine(v6, v7, color, color);
    drawLine(v7, v4, color, color);

    // Vertical Lines
    drawLine(v0, v4, color, color);
    drawLine(v1, v5, color, color);
    drawLine(v2, v6, color, color);
    drawLine(v3, v7, color, color);
}

void BatchRenderer::loadResources(ResourceManager* resource) {
    resource->loadModelFromFile("Data/Models/camera.glb", true);

    cameraMesh = resource->getMesh("Camera");

    viz.Init(resource);
}







/* Render Profiling */
void BatchRenderer::beginProfile() {
    dur_fullRenderPass = 0;
    profileStart = _clock::now();
}
BatchRenderer::_dur BatchRenderer::profileRenderPass() {
    _dur duration =
        std::chrono::duration_cast<std::chrono::microseconds>(
            _clock::now() - profileStart).count();

    dur_fullRenderPass += duration;

    profileStart = _clock::now();
    return duration;
}
void BatchRenderer::endProfile() {
    avgRenderPass.addSample(dur_fullRenderPass);
}
