#ifndef SHADOWMAP_H
#define SHADOWMAP_H

#include "glad\glad.h"
#include "GameMath.hpp"

class Shadowmap {
public:
    Shadowmap();

    void create(u32 width, u32 height);
    void bind(GLenum tex_unit);

//private:
    GLuint depthMapFBO;
    GLuint depthMap;

    static void initShadows();
    static math::mat4 lightProjection;
};

#endif
