#ifndef GBUFFER_H
#define GBUFFER_H

#include "glad/glad.h"
#include "GameMath.hpp"

class GBuffer {
public:
    GBuffer();

    // Common
    void resize(u32 width, u32 height);
    void unbind();

    virtual void bind();
    virtual void create(u32 width, u32 height);
    virtual void cleanup();

    /* Render targets
    R       G       B       A
    Target 0:   [     Albedo    ][ metallic  ] LDR
    Target 1:   [     Normal    ][ roughness ] LDR
    Target 2:   [    Emission   ][    ao     ] HDR(ao is LDR)
    */
    GLuint rt0, rt1, rt2;

protected:
    GLuint fbo;
    GLuint rbo;

    u32 m_width, m_height;
};

#endif