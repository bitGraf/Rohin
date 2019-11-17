#include "RenderManager.hpp"

RenderManager::RenderManager() {
    fps = 0;
}

RenderManager::~RenderManager() {

}

void RenderManager::update(double dt) {

}

void RenderManager::handleMessage(Message msg) {
    if (msg.isType("InputMouseButton")) {
        // int button, int action, int mods
        using dt = Message::Datatype;
        dt button   = msg.data[0];
        dt action   = msg.data[1];
        dt mods     = msg.data[2];
        dt xPos     = msg.data[3];
        dt yPos     = msg.data[4];

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            Console::logMessage("Clicked: " + std::to_string(xPos) + ", " + std::to_string(yPos));

            //vec3 worldPos = camera->getWorldPos(xPos, yPos);
        }
    }

    if (msg.isType("InputKey")) {
        // int button, int action, int mods
        using dt = Message::Datatype;
        dt key = msg.data[0];
        dt scancode = msg.data[1];
        dt action = msg.data[2];
        dt mods = msg.data[3];

        if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {
            Console::logMessage("Reloading Shaders");

            m_mainShader.create("static_pbr_withShadow.vert", "static_pbr_withShadow.frag", "mainShader_withShadows");
            m_lineShader.create("line.vert", "line.frag", "lineShader");
            m_skyboxShader.create("skybox.vert", "skybox.frag", "skyboxShader");
            m_fullscreenShader.create("fullscreen_quad.vert", "fullscreen_quad.frag", "fullscreenShader");
            m_shadowShader.create("shadow.vert", "shadow.frag", "shadowShader");
        }
    }
}

void RenderManager::destroy() {

}

CoreSystem* RenderManager::create() {
    //m_mainShader.create("static_pbr.vert", "static_pbr.frag", "mainShader");
    m_mainShader.create("static_pbr_withShadow.vert", "static_pbr_withShadow.frag", "mainShader_withShadows");
    m_lineShader.create("line.vert", "line.frag", "lineShader");
    m_skyboxShader.create("skybox.vert", "skybox.frag", "skyboxShader");
    m_fullscreenShader.create("fullscreen_quad.vert", "fullscreen_quad.frag", "fullscreenShader");
    m_shadowShader.create("shadow.vert", "shadow.frag", "shadowShader");

    blackTex.loadImage("black.png");
    whiteTex.loadImage("white.png");
    normalTex.loadImage("normal.png");

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

    // Init framebuffer
    fb.create(800, 600);

    Shadowmap::initShadows();
    sm.create(2048, 2048); // resolution of shadow map

    DynamicFont::InitTextRendering(800, 600);
    font.create("UbuntuMono-Regular.ttf", 20);

    return this;
}

void RenderManager::renderScene(Window* window, Scene* scene) {
    //window->makeCurrent();
    // Update camera once
    scene->camera.updateProjectionMatrix(window->m_width, window->m_height);
    //scene->camera.updateViewMatrix();

    // First, render to depth map
    m_shadowShader.use();
    glViewport(0, 0, 1024, 1024);
    glBindFramebuffer(GL_FRAMEBUFFER, sm.depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        
        mat4 lightView;
        lightView.lookAt(-scene->sun.direction.get_unit()*5, vec3(), vec3(0,1,0));
        m_shadowShader.setMat4("viewMatrix", lightView);
        m_shadowShader.setMat4("projectionMatrix", Shadowmap::lightProjection);
        for (auto ent : scene->m_entities) {
            // Render entity ent
            if (ent.m_mesh != nullptr) {
                setCurrentMesh(ent.m_mesh);
                setTransforms(&m_shadowShader, &ent.position, &ent.orientation, &ent.scale);
                renderPrimitive(ent.m_mesh);
            }
        }

    //renderscene
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, window->m_width, window->m_height);

    // Update framebuffer
    fb.resize(window->m_width, window->m_height);
    fb.bind();
    
    // Then, render to framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    /* Render Line Entities */
    m_lineShader.use();
    setCamera(&m_lineShader, &scene->camera);
    //renderGrid(&m_lineShader, *scene->gridVAO, *scene->numVerts);

    /* Render Poly Entities */
    m_mainShader.use();
    m_mainShader.setLights(
        &scene->sun,
        scene->pointLights,
        scene->spotLights
    ); // TODO: remove this
    setCamera(&m_mainShader, &scene->camera);

    m_mainShader.setInt("irradianceMap", 5);
    m_mainShader.setInt("prefilterMap", 6);
    m_mainShader.setInt("brdfLUT", 7);
    m_mainShader.setInt("shadowMap", 8);

    m_mainShader.setMat4("lightProjectionMatrix", Shadowmap::lightProjection);
    m_mainShader.setMat4("lightViewMatrix", lightView);

    scene->envMap.bindPBR(GL_TEXTURE5, GL_TEXTURE6, GL_TEXTURE7);
    sm.bind(GL_TEXTURE8);

    /* Render Entities */
    for (auto ent : scene->m_entities) {
        // Render entity ent
        if (ent.m_mesh != nullptr) {
            setCurrentMesh(ent.m_mesh);
            if (ent.m_material != nullptr) {
                setCurrentMaterial(&m_mainShader, ent.m_material);
            }

            setTransforms(&m_mainShader, &ent.position, &ent.orientation, &ent.scale);
            renderPrimitive(ent.m_mesh);
        }
    }

    /* Render skybox */
    m_skyboxShader.use();
    renderSkybox(&m_skyboxShader, &scene->camera, &scene->envMap);

    fb.unbind();

    /* Render fullscreen quad */
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_fullscreenShader.use();
    m_fullscreenShader.setInt("tex", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fb.getTexture());
    //glBindTexture(GL_TEXTURE_2D, sm.depthMap);

    glBindVertexArray(fullscreenVAO);
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);

    vec4 white(1, 1, 1, 1);
    char fpsText[64];
    sprintf(fpsText, "FPS: %.1f", fps);
    font.drawText(window->m_width-5, 5, white, fpsText, ALIGN_TOP_RIGHT);

    char rpyText[64];
    sprintf(rpyText, "Roll:  %.3f deg", scene->camera.roll);
    font.drawText(window->m_width - 5, 55, white, rpyText, ALIGN_TOP_RIGHT);
    sprintf(rpyText, "Pitch: %.3f deg", scene->camera.pitch);
    font.drawText(window->m_width - 5, 75, white, rpyText, ALIGN_TOP_RIGHT);
    sprintf(rpyText, "Yaw:   %.3f deg", scene->camera.yaw);
    font.drawText(window->m_width - 5, 95, white, rpyText, ALIGN_TOP_RIGHT);

    sprintf(rpyText, "Pos:   (%.2f,%.2f,%.2f)", scene->camera.position.x, scene->camera.position.y, scene->camera.position.z);
    font.drawText(window->m_width - 5, 115, white, rpyText, ALIGN_TOP_RIGHT);
}

void RenderManager::renderEditor(Window* window, Scene* scene) {
    /* Render fullscreen quad */
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_fullscreenShader.use();
    m_fullscreenShader.setInt("tex", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fb.getTexture());

    glBindVertexArray(fullscreenVAO);
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);

    char selectName[128] = "Selected: ";

    vec2 cursorPos = window->getCursorPos();
    int n = 0;
    for (auto pick : scene->m_entities) {
        mat4 modelMatrix = (
            mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), vec4(pick.position, 1)) *
            mat4(pick.orientation) *
            mat4(pick.scale.x, pick.scale.y, pick.scale.z, 1));

        pick.calcScreenSpace(
            scene->camera.projectionMatrix,
            scene->camera.viewMatrix,
            modelMatrix,
            vec3(),
            window->m_width, window->m_height);

        vec2 screenPos = vec2(pick.screenPos.x, pick.screenPos.y);

        f32 dist = (screenPos - cursorPos).length();
        if (dist < pick.pickRadius) {
            sprintf(selectName, "%s[%s]", selectName, pick.name.c_str());
        }
    }

    vec4 white(1, 1, 1, 1);
    font.drawText(5, 5, white, "Editor");
    char text[128];
    sprintf(text, "Cursor pos: %.0f, %.0f", cursorPos.x, cursorPos.y);
    font.drawText(5, 595, white, text, ALIGN_BOT_LEFT);
    font.drawText(795, 595, white, selectName, ALIGN_BOT_RIGHT);
    char fpsText[64];
    sprintf(fpsText, "FPS: %.1f", fps);
    font.drawText(window->m_width-5, 5, white, fpsText, ALIGN_TOP_RIGHT);
}


void RenderManager::setCamera(Shader* shader, Camera* camera) {
    shader->setMat4("viewMatrix", camera->viewMatrix);
    shader->setMat4("projectionMatrix", camera->projectionMatrix);
    shader->setVec3("camPos", camera->position);
}

void RenderManager::setCurrentMesh(const TriangleMesh* mesh) {
    glBindVertexArray(mesh->VAO);
}

void RenderManager::setCurrentMaterial(Shader* shader, const Material* material) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material->baseColorTexture.glTexID == 0 ? blackTex.glTextureID : material->baseColorTexture.glTexID);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, material->normalTexture.glTexID == 0 ? normalTex.glTextureID : material->normalTexture.glTexID);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, material->metallicRoughnessTexture.glTexID == 0 ? blackTex.glTextureID : material->metallicRoughnessTexture.glTexID);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, material->occlusionTexture.glTexID == 0 ? whiteTex.glTextureID : material->occlusionTexture.glTexID);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, material->emissiveTexture.glTexID == 0 ? blackTex.glTextureID : material->emissiveTexture.glTexID);

    shader->setMaterial("material", material);
}

void RenderManager::setTransforms(
    Shader* shader,
    const math::vec3* pos,
    const math::mat3* orientation,
    const math::vec3* scale) {

    mat4 modelMatrix = (
        mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), vec4(*pos, 1)) *
        mat4(*orientation) * 
        mat4(scale->x, scale->y, scale->z, 1));

    shader->setMat4("modelMatrix", modelMatrix);
}

void RenderManager::renderPrimitive(const TriangleMesh* mesh) {
    glDrawElements(GL_TRIANGLES, mesh->numFaces*3, GL_UNSIGNED_SHORT, 0);
}

void RenderManager::renderGrid(Shader* shader, GLuint vao, GLuint numVerts) {
    shader->setVec4("color", vec4(0,1,0,1));
    glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, numVerts);
}

void RenderManager::renderSkybox(Shader* shader, Camera* camera, EnvironmentMap* skybox) {
    shader->setMat4("viewMatrix", mat4(mat3(camera->viewMatrix)));
    shader->setMat4("projectionMatrix", camera->projectionMatrix);

    shader->setInt("skybox", 0);
    //skybox->bind(GL_TEXTURE0);
    skybox->bindSkybox(GL_TEXTURE0);

    glDepthFunc(GL_LEQUAL);
    glBindVertexArray(skybox->skyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);
}

void RenderManager::lastFrameTime(long long k) {
    fps = 1000000.0 / static_cast<f32>(k);
}