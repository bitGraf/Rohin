#include "UIElement.hpp"

UIElement::UIElement() {
}

void UIElement::create() {
    tex.loadImage("GiantToad.png");

    screenPos = vec2(0, 0);
    scale = vec2(0.5, 0.5);
    rotation = 0;
}
