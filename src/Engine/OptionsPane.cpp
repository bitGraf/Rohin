#include "OptionsPane.hpp"

/*
OptionsPane::OptionsPane() {
    Visibility = false;
}

void OptionsPane::create(Window* mainWin, bool Show) {
    Visibility = Show;

    m_pane.create_window("Options", 300, 500, !Show);
    m_pane.setPosition(20, 600);
    m_pane.makeCurrent();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    textFont.InitTextRendering();
    textFont.create("UbuntuMono-Regular.ttf", 20, 300, 500);

    m_main = mainWin;
    ySpacing = textFont.m_fontSize*spacing;

    nextBound = vec4(xSetting, 0, 300, ySpacing);
    // Create settings list
    toggleSettings.push_back(
        toggleSet("Render", 5,
            toggle("Wireframe Mode", &g_options.wireFrameMode, getNextBound()),
            toggle("Draw Shadows", &g_options.drawShadows, getNextBound()),
            toggle("Draw Skybox", &g_options.drawSkybox, getNextBound()),
            toggle("Draw Static Entities", &g_options.drawStaticEntities, getNextBound()),
            toggle("Draw Dynamic Entities", &g_options.drawDynamicEntities, getNextBound()))
    );
    nextToggleSet();
    toggleSettings.push_back(
        toggleSet("Scene", 1,
            toggle("Update Scene", &g_options.updateScene, getNextBound()))
    );
    nextToggleSet();
    toggleSettings.push_back(
        toggleSet("Debug", 2,
            toggle("Draw Fps", &g_options.drawFps, getNextBound()),
            toggle("Limit Framerate", &g_options.limitFramerate, getNextBound()))
    );
    for (int n = 0; n < toggleSettings.size(); n++) {
        std::reverse(toggleSettings[n].settings.begin(), toggleSettings[n].settings.end());
    }
}

void OptionsPane::redraw() {
    if (Visibility) {
        m_pane.makeCurrent();

        //glClearColor(.4, .3, .2, 1);
        glClearColor(255.0f/255.0f, 248.0f/255.0f, 231.0f/255.0f, 1.0f);//Cosmic Latte, too bright :(

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        math::vec4 low = math::vec4(.5, .1, .1, 1);
        math::vec4 high = math::vec4(.1, .5, .1, 1);
        math::vec4 header = math::vec4(.1, .1, .1, 1);
        float x = xHeader;
        float y = 0;

        for (auto tSet : toggleSettings) {
            textFont.drawText(x, y, header, (char*)tSet.name.c_str());

            y += ySpacing;
            x = xSetting;

            for (auto t : tSet.settings) {
                textFont.drawText(x, y, (*t.value)?high:low, (char*)t.name.c_str());
                y += ySpacing;
            }

            y += ySpacing *0.25;
            x = xHeader;
        }

        m_pane.swapAndPoll();

        m_main->makeCurrent();
    }
}

void OptionsPane::click(s32 key, s32 action) {
    if (Visibility) {
        if (m_pane.cursorOver) {
            for (auto ts : toggleSettings) {
                for (auto t : ts.settings) {
                    if (inBounds(t.bounds, vec2(x, y))) {
                        Console::logMessage(t.name);
                        (*t.value) = !(*t.value);
                    }
                }
            }
            redraw();
        }
    }
}

void OptionsPane::nextToggleSet() {
    nextBound.y += ySpacing * 1.25;
    nextBound.w += ySpacing * 1.25;
}

vec4 OptionsPane::getNextBound() {
    nextBound.y += ySpacing;
    nextBound.w += ySpacing;

    return nextBound;
}

bool OptionsPane::inBounds(vec4 bounds, vec2 loc) {
    if (loc.x > bounds.x && loc.x < bounds.z) {
        if (loc.y > bounds.y && loc.y < bounds.w) {
            return true;
        }
    }
    return false;
}

void OptionsPane::ToggleVisibility() {
    Visibility = !Visibility;

    if (!Visibility) {
        glfwHideWindow(m_pane.m_glfwWindow);
    }
    else {
        glfwShowWindow(m_pane.m_glfwWindow);
        this->redraw();
    }
}
*/