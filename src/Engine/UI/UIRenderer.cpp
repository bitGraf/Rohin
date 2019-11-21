#include "UIRenderer.hpp"

UIRenderer::UIRenderer() {
}

UIRenderer::~UIRenderer() {

}

void UIRenderer::update(double dt) {

}

void UIRenderer::handleMessage(Message msg) {
    if (msg.isType("InputMouseButton")) {
        // int button, int action, int mods
        using dt = Message::Datatype;
        dt button   = msg.data[0];
        dt action   = msg.data[1];
        dt mods     = msg.data[2];
        dt xPos     = msg.data[3];
        dt yPos     = msg.data[4];
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

            m_UIShader.create("UI.vert", "UI.frag", "uiShader");
        }
    }
}

void UIRenderer::destroy() {

}

CoreSystem* UIRenderer::create() {
    m_UIShader.create("UI.vert", "UI.frag", "uiShader");

    const float UIElementVerts[] = {
        0, 0,
        0, 1,
        1, 0,

        1, 0,
        0, 1,
        1, 1
    };

    GLuint UIElementVBO;
    glGenVertexArrays(1, &UIElementVAO);
    glGenBuffers(1, &UIElementVBO);
    glBindVertexArray(UIElementVAO);
    glBindBuffer(GL_ARRAY_BUFFER, UIElementVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(UIElementVerts),
        &UIElementVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(f32), (void*)0);

    glBindVertexArray(0);

    // Create UI elements
    UIElement ui;
    ui.create();

    // Test Alignment system
    ui.position = vec2(0, 0);
    ui.scale = vec2(0.1, 0.1);
    ui.align = UIAlignment::ALIGN_TOP_LEFT;
    m_uiElements.push_back(ui);

    ui.position = vec2(0, 0.5);
    ui.scale = vec2(0.1, 0.2);
    ui.align = UIAlignment::ALIGN_MID_LEFT;
    m_uiElements.push_back(ui);

    ui.position = vec2(0, 1);
    ui.scale = vec2(0.1, 0.1);
    ui.align = UIAlignment::ALIGN_BOT_LEFT;
    m_uiElements.push_back(ui);


    ui.position = vec2(0.5, 0);
    ui.scale = vec2(0.2, 0.1);
    ui.align = UIAlignment::ALIGN_TOP_MID;
    m_uiElements.push_back(ui);

    ui.position = vec2(0.5, 0.5);
    ui.scale = vec2(0.1, 0.1);
    ui.align = UIAlignment::ALIGN_MID_MID;
    m_uiElements.push_back(ui);

    ui.position = vec2(0.5, 1);
    ui.scale = vec2(0.2, 0.1);
    ui.align = UIAlignment::ALIGN_BOT_MID;
    m_uiElements.push_back(ui);


    ui.position = vec2(1, 0);
    ui.scale = vec2(0.1, 0.1);
    ui.align = UIAlignment::ALIGN_TOP_RIGHT;
    m_uiElements.push_back(ui);

    ui.position = vec2(1, 0.5);
    ui.scale = vec2(0.1, 0.2);
    ui.align = UIAlignment::ALIGN_MID_RIGHT;
    m_uiElements.push_back(ui);

    ui.position = vec2(1, 1);
    ui.scale = vec2(0.1, 0.1);
    ui.align = UIAlignment::ALIGN_BOT_RIGHT;
    m_uiElements.push_back(ui);


    lensFlares[0].loadImage("LensFlare/sun.png");
    for (int n = 1; n < 10; n++) {
        lensFlares[n].loadImage("LensFlare/tex" + std::to_string(n) + ".png");
    }

    return this;
}

void UIRenderer::renderScene(Window* window, Scene* scene) {
    glDisable(GL_DEPTH_TEST);
    m_UIShader.use();

    glBindVertexArray(UIElementVAO);
    m_UIShader.setInt("tex", 0);

    // Draw all UI elements
    for (auto ui : m_uiElements) {
        vec2 offset = getAlignmentOffset(ui.align, ui.scale);
        m_UIShader.setVec2("pos", ui.position);
        m_UIShader.setVec2("offset", offset);
        m_UIShader.setVec2("scale", ui.scale);
        m_UIShader.setFloat("factor", 1 - ui.transparency);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ui.tex.glTextureID);

        //glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    drawLensFlare(scene);

    glEnable(GL_DEPTH_TEST);
}

void UIRenderer::drawLensFlare(Scene* scene) {
    auto cam = &scene->camera;

    vec3 sunPosWorld = -scene->sun.direction.get_unit() * 50;
    vec4 sunScreenPos = cam->projectionMatrix * cam->viewMatrix * vec4(sunPosWorld, 1);
    vec2 screenPos = (vec2(sunScreenPos.x / sunScreenPos.w, sunScreenPos.y / sunScreenPos.w) / 2) + vec2(.5);
    screenPos = vec2(screenPos.x, 1 - screenPos.y);
    vec2 sun2center = vec2(.5, .5) - screenPos;
    scalar dist = (sun2center).length();
    float brightness = 1 - (dist / .6);
    vec2 scale = vec2(.25);
    vec2 alignOff = getAlignmentOffset(UIAlignment::ALIGN_MID_MID, scale);

    // Draw lens flare system
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    m_UIShader.setFloat("factor", brightness);
    m_UIShader.setVec2("offset", alignOff);
    m_UIShader.setVec2("scale", scale);

    if (sunScreenPos.w > 0 && brightness > 0) {
        // draw the lens flare
        int num = 7;
        scalar spacing = .4;
        vec2 offset = sun2center * spacing;
        for (int n = 0; n < 10; n++) {
            vec2 pos = screenPos + offset * ((scalar)n);

            m_UIShader.setVec2("pos", pos);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, lensFlares[n].glTextureID);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        // Draw the sun
        m_UIShader.setVec2("pos", screenPos);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, lensFlares[0].glTextureID);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glBindVertexArray(0);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}