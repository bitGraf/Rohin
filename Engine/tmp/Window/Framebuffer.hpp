#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "glad/glad.h"
#include "Engine/Core/GameMath.hpp"

#include <unordered_map>

enum class ResolutionScale {
    One = 1,
    OneHalf,
    OneThird,
    OneFourth,
    Twice,
    Triple,
    Quadruple
};

class Framebuffer_new {
public:
    Framebuffer_new(u32 width, u32 height);

    void resize(u32 width, u32 height);
    void bind();
    void setResolution();
    void resetResolution();
    void unbind();

    math::vec2 getRenderSize();

    void addRenderBufferObject();
    void addColorBufferObject(std::string name, unsigned int num,
        GLint internalFormat, GLenum format, GLenum type);
    GLuint getColorBuffer(std::string name);

    void create(ResolutionScale scale = ResolutionScale::One);
    void destroy();

private:
    struct ColorAttachmentData {
        GLint internalFormat;
        GLenum format;
        GLenum type;
        GLenum attachment;

        GLuint glBuffer;
    };

    std::unordered_map<std::string, ColorAttachmentData> colorBuffers;
    GLuint renderBuffer;
    bool hasRenderBuffer;

    GLuint fbo;
    u32 base_width, base_height; //screen resolution
    ResolutionScale scale;

    math::vec2 getResolution(ResolutionScale scale);
};

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