#include "Shadowmap.hpp"

math::mat4 Shadowmap::lightProjection;

Shadowmap::Shadowmap() {

}

void Shadowmap::initShadows() {
    f32 zNear = 1.0, zFar = 7.5;

    /*
    m->a11 = 0.1;
    m->a22 = 0.1;
    m->a33 = -2/(zfar-znear);
    m->a44 = 1;

    m->a34 = -(zfar + znear) / (zfar-znear);
    */

    using namespace math;
    lightProjection = mat4(
        vec4(0.1,0,0,0),
        vec4(0,0.1,0,0),
        vec4(0,0,-2/(zFar-zNear),0),
        vec4(0,0,-(zFar+zNear)/(zFar-zNear),1)
    );
}

void Shadowmap::create(u32 width, u32 height) {
    glGenFramebuffers(1, &depthMapFBO);

    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // attach texture to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Shadowmap::bind(GLenum tex_unit) {
    glActiveTexture(tex_unit);
    glBindTexture(GL_TEXTURE_2D, depthMap);
}