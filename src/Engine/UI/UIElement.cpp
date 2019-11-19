#include "UIElement.hpp"

UIElement::UIElement() {
}

void UIElement::create() {
    tex.loadImage("GiantToad.png");

    position = vec2(0, 0);
    scale = vec2(0.5, 0.5);
    transparency = 0;
    align = UIAlignment::ALIGN_TOP_LEFT;
}

vec2 getAlignmentOffset(UIAlignment align, vec2 scale) {
    switch (align) {
        case UIAlignment::ALIGN_TOP_LEFT: {
            return vec2(0, 0);
        } break;
        case UIAlignment::ALIGN_TOP_MID: {
            return vec2(-scale.x/2, 0);
        } break;
        case UIAlignment::ALIGN_TOP_RIGHT: {
            return vec2(-scale.x, 0);
        } break;

        case UIAlignment::ALIGN_MID_LEFT: {
            return vec2(0, scale.y/2);
        } break;
        case UIAlignment::ALIGN_MID_MID: {
            return vec2(-scale.x / 2, scale.y / 2);
        } break;
        case UIAlignment::ALIGN_MID_RIGHT: {
            return vec2(-scale.x, scale.y / 2);
        } break;

        case UIAlignment::ALIGN_BOT_LEFT: {
            return vec2(0, scale.y);
        } break;
        case UIAlignment::ALIGN_BOT_MID: {
            return vec2(-scale.x / 2, scale.y);
        } break;
        case UIAlignment::ALIGN_BOT_RIGHT: {
            return vec2(-scale.x, scale.y);
        } break;
    }
}