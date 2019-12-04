#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "glad\glad.h"
#include "GameMath.hpp"

class Framebuffer {
public:
    Framebuffer();

    // Common
    void resize(u32 width, u32 height);
    void unbind();

    virtual void bind();
    virtual void create_LDR(u32 width, u32 height);
    virtual void create(u32 width, u32 height);
    virtual void cleanup();
    virtual GLuint getTexture() { return texture; }

protected:
    GLuint fbo;
    GLuint rbo;
    GLuint texture;

    u32 m_width, m_height;
};

class Framebuffer_color_pos : public Framebuffer {
public:
    Framebuffer_color_pos();

    void create(u32 width, u32 height);
    void cleanup();

    GLuint getPositionTex() { return gPosition; }

protected:
    GLuint gPosition;
};

class Framebuffer_swap : public Framebuffer {
public:
    Framebuffer_swap();

    void bind();
    void create(u32 width, u32 height);
    void cleanup();

    void swap();

    GLuint getTexture();

protected:
    GLuint fbo2;
    GLuint rbo2;
    GLuint texture2;

    bool onBuffer1;
};

#endif