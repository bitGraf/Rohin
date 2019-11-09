#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "glad\glad.h"
#include "GameMath.hpp"

class Framebuffer {
public:
    Framebuffer();

    void create(u32 width, u32 height);
    void resize(u32 width, u32 height);
    void cleanup();
    void bind();
    void unbind();

    GLuint getTexture() { return texture; }

private:
    GLuint fbo;
    GLuint rbo;
    GLuint texture;

    u32 m_width, m_height;
};

#endif