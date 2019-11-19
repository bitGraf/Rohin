#ifndef UI_ELEMENT_H
#define UI_ELEMENT_H

#include "GameMath.hpp"
#include "glad\glad.h"
#include "Resource\Texture.hpp"

using namespace math;

class UIElement {
public:
    UIElement();

    void create();

    vec2 screenPos; // ([0,1],[0,1])
    vec2 scale; //[0,1]
    scalar rotation;

    Texture tex;
};

#endif
