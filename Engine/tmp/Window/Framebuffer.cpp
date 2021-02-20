#include "rhpch.hpp"
#include "Framebuffer.hpp"

Framebuffer_new::Framebuffer_new(u32 width, u32 height) :
    base_width(width),
    base_height(height),
    fbo(0),
    renderBuffer(0),
    hasRenderBuffer(false)
{}

void Framebuffer_new::resize( u32 width, u32 height) {
    if (base_width == width && base_height == height) {
        //do nothing
        return;
    }

    base_width = width;
    base_height = height;
    destroy();
    create(scale);
}

void Framebuffer_new::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    setResolution();
}
void Framebuffer_new::unbind() {
    resetResolution();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Framebuffer_new::setResolution() {
    math::vec2 res = getResolution(scale);
    glViewport(0, 0, (GLsizei)res.x, (GLsizei)res.y);
}
void Framebuffer_new::resetResolution() {
    glViewport(0, 0, base_width, base_height);
}
math::vec2 Framebuffer_new::getRenderSize() {
    return getResolution(scale);
}
GLuint Framebuffer_new::getColorBuffer(std::string name) {
    auto f = colorBuffers.find(name);
    if (f == colorBuffers.end()) {
        return 0;
    }
    else {
        return f->second.glBuffer;
    }
}

void Framebuffer_new::addColorBufferObject(std::string name, unsigned int num,
    GLint internalFormat, GLenum format, GLenum type) {

    Framebuffer_new::ColorAttachmentData data;

    data.format = format;
    data.internalFormat = internalFormat;
    data.type = type;
    data.glBuffer = 0;
    data.attachment = GL_COLOR_ATTACHMENT0 + num;

    // add to list
    colorBuffers[name] = data;
}

void Framebuffer_new::addRenderBufferObject() {
    hasRenderBuffer = true;
}

void Framebuffer_new::create(ResolutionScale scale) {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    this->scale = scale;
    math::vec2 res = this->getResolution(scale);

    // Create color buffers
    for (auto buff : colorBuffers) {
        // Temp. Position buffer
        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);

        auto x = (GLsizei)res.x;
        auto y = (GLsizei)res.y;

        glTexImage2D(GL_TEXTURE_2D, 0, buff.second.internalFormat,
            (GLsizei)res.x, (GLsizei)res.y, 0, buff.second.format, 
            buff.second.type, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, buff.second.attachment, GL_TEXTURE_2D, tex, 0);

        colorBuffers[buff.first].glBuffer = tex;
    }

    // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    unsigned int attachments[16];
    for (int n = 0; n < 16; n++) {
        attachments[n] = GL_COLOR_ATTACHMENT0 + n;
    }
   
    glDrawBuffers(colorBuffers.size(), attachments);

    // Create renderbuffer
    if (hasRenderBuffer) {
        //Create framebuffer
        GLuint rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, (GLsizei)res.x, (GLsizei)res.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

        renderBuffer = rbo;
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer_new::destroy() {
    glDeleteFramebuffers(1, &fbo);

    glDeleteRenderbuffers(1, &renderBuffer);
    for (auto k : colorBuffers) {
        glDeleteTextures(1, &k.second.glBuffer);
    }
}

math::vec2 Framebuffer_new::getResolution(ResolutionScale scale) {
    switch (scale) {
    case ResolutionScale::One:
        return math::vec2(base_width, base_height);
    case ResolutionScale::OneHalf:
        return math::vec2(base_width/2.0f, base_height/2.0f);
    case ResolutionScale::OneThird:
        return math::vec2(base_width/3.0f, base_height/3.0f);
    case ResolutionScale::OneFourth:
        return math::vec2(base_width/4.0f, base_height/4.0f);
    case ResolutionScale::Twice:
        return math::vec2(base_width*2.0f, base_height*2.0f);
    case ResolutionScale::Triple:
        return math::vec2(base_width*3.0f, base_height*3.0f);
    case ResolutionScale::Quadruple:
        return math::vec2(base_width*4.0f, base_height*4.0f);
    }
}





Framebuffer::Framebuffer() {
}

void Framebuffer::create(u32 width, u32 height) {
    m_width = width;
    m_height = height;

    glGenFramebuffers(1, &fbo);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    //Attach texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,
        width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);    

    // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);

    //Create framebuffer
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::create_LDR(u32 width, u32 height) {
    m_width = width;
    m_height = height;

    glGenFramebuffers(1, &fbo);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    //Attach texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);

    //Create framebuffer
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::cleanup() {
    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteTextures(1, &texture);
}

void Framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void Framebuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::resize(u32 width, u32 height) {
    if (m_width == width && m_height == height) {
        //do nothing
        return;
    }

    cleanup();
    create(width, height);
}




Framebuffer_color_pos::Framebuffer_color_pos() {
}

void Framebuffer_color_pos::create(u32 width, u32 height) {
    m_width = width;
    m_height = height;

    glGenFramebuffers(1, &fbo);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    //Attach texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,
        width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Attach position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gPosition, 0);


    // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    //Create framebuffer
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer_color_pos::cleanup() {
    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteTextures(1, &texture);
    glDeleteTextures(1, &gPosition);
}









Framebuffer_swap::Framebuffer_swap() {
    onBuffer1 = true;
}

void Framebuffer_swap::create(u32 width, u32 height) {
    Framebuffer::create(width, height);

    m_width = width;
    m_height = height;

    glGenFramebuffers(1, &fbo2);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo2);

    //Attach texture
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,
        width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture2, 0);

    // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);

    //Create framebuffer
    glGenRenderbuffers(1, &rbo2);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo2);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo2);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer_swap::cleanup() {
    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteTextures(1, &texture);

    glDeleteFramebuffers(1, &fbo2);
    glDeleteRenderbuffers(1, &rbo2);
    glDeleteTextures(1, &texture2);
}

void Framebuffer_swap::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, onBuffer1 ? fbo : fbo2);
}

void Framebuffer_swap::swap() {
    onBuffer1 = !onBuffer1;
}

GLuint Framebuffer_swap::getTexture() {
    return onBuffer1 ? texture2 : texture; // if FBO 1 is active, return texture2
}
