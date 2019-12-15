#ifndef UI_ELEMENT_H
#define UI_ELEMENT_H

#include "GameMath.hpp"
#include "glad/glad.h"
#include "Resource/Texture.hpp"

using namespace math;

enum class UIAlignment {
    ALIGN_TOP_LEFT = 0,
    ALIGN_MID_LEFT,
    ALIGN_BOT_LEFT,
    ALIGN_TOP_MID,
    ALIGN_MID_MID,
    ALIGN_BOT_MID,
    ALIGN_TOP_RIGHT,
    ALIGN_MID_RIGHT,
    ALIGN_BOT_RIGHT
};

vec2 getAlignmentOffset(UIAlignment align, vec2 scale);

class UIElement {
public:
    UIElement();

    void create();

    vec2 position; // ([0,1],[0,1])
    vec2 scale; // ([0,1],[0,1])
    scalar transparency; // [0,1]

    Texture tex;
    UIAlignment align;
};

#endif
