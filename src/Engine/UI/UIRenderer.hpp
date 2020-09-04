#ifndef UIRENDER_MANAGER_H
#define UIRENDER_MANAGER_H

#include "Window/Window.hpp"
#include "Render/Shader.hpp"
#include "UIElement.hpp"
#include "Scene/Scene.hpp"

class UIRenderer {
public:
    UIRenderer();
    ~UIRenderer();

    void update(double dt);
    void destroy();
        
    void renderScene(Window* window, Scene* scene);

    std::vector<UIElement> m_uiElements;
    Texture lensFlares[10];

private:
    void drawLensFlare(Scene* scene);

    Shader m_UIShader;
    GLuint UIElementVAO;
};

#endif
