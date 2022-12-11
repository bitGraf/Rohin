#include <enpch.hpp>
#include "OpenGLGraphicsContext.hpp"

#include <glfw/glfw3.h>
#include <glad/glad.h>

namespace rh {

    OpenGLGraphicsContext::OpenGLGraphicsContext(GLFWwindow* windowHandle) 
        : m_WindowHandle(windowHandle) {
        ENGINE_LOG_ASSERT(windowHandle, "Window handle does not exist");
    }

    void OpenGLGraphicsContext::Init() {
        glfwMakeContextCurrent(m_WindowHandle);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        ENGINE_LOG_ASSERT(status, "Failed to initialize GLAD");

        ENGINE_LOG_INFO("OpenGL Info:");
        ENGINE_LOG_INFO("  Vendor:   {0}", glGetString(GL_VENDOR));
        ENGINE_LOG_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
        ENGINE_LOG_INFO("  Version:  {0}", glGetString(GL_VERSION));
        ENGINE_LOG_INFO("  Version:  {0}", glGetString(GL_SHADING_LANGUAGE_VERSION));
    }

    void OpenGLGraphicsContext::SwapBuffers() {
        glfwSwapBuffers(m_WindowHandle);
    }

    const unsigned char* OpenGLGraphicsContext::GetVendorString() const {
        return glGetString(GL_VENDOR);
    }
    const unsigned char* OpenGLGraphicsContext::GetDeviceString() const {
        return glGetString(GL_RENDERER);
    }
    const unsigned char* OpenGLGraphicsContext::GetVersionString() const {
        return glGetString(GL_VERSION);
    }
    const unsigned char* OpenGLGraphicsContext::GetAPIVersionString() const {
        return glGetString(GL_SHADING_LANGUAGE_VERSION);
    }
}