#include "EnvironmentMap.hpp"

EnvironmentMap::EnvironmentMap() {

}

void EnvironmentMap::loadHDRi(std::string filename) {
    stbi_set_flip_vertically_on_load(false);

    int imgWidth, imgHeight, nrChannels;
    float *data = stbi_loadf((HdrResourcePath + filename).c_str(), &imgWidth, &imgHeight, &nrChannels, 0);
    if (data) {
        glGenTextures(1, &hdrEquirect);
        glBindTexture(GL_TEXTURE_2D, hdrEquirect);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, imgWidth, imgHeight, 0, GL_RGB, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Failed to load HDR image [" << filename << "." << std::endl;
    }
}

void EnvironmentMap::bind(GLenum tex_unit) {
    glActiveTexture(tex_unit);
    //glBindTexture(GL_TEXTURE_2D, hdrEquirect);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
}

void EnvironmentMap::initShaders() {
    cubemapConverter.create("cubemapConvert.vert", "cubemapConvert.frag", "cubemapConverter");

    captureCam.m_fov = 90;
    captureCam.m_zFar = 10;
    captureCam.m_zNear = .1;
    captureCam.m_aspectRatio = 1;
    captureCam.updateProjectionMatrix(100, 100);

    captureCam.position = vec3(0.0);
    captureViews[0] = captureCam.lookAt(vec3(0, 0,  1), true).viewMatrix;
    captureViews[1] = captureCam.lookAt(vec3(0, 0, -1), true).viewMatrix;
    captureViews[2] = captureCam.lookAt(vec3(0,  1, 0), true).viewMatrix;
    captureViews[3] = captureCam.lookAt(vec3(0, -1, 0), true).viewMatrix;
    captureViews[4] = captureCam.lookAt(vec3( 1, 0, 0), true).viewMatrix;
    captureViews[5] = captureCam.lookAt(vec3(-1, 0, 0), true).viewMatrix;

    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);
}

void EnvironmentMap::preCompute() {
    SkyboxRes = 1024;
    irradianceRes = 32;
    prefilterRes = 128;
    brdfLUTRes = 512;

    viewportWidth = 1280;
    viewportHeight = 720;

    initShaders();
    convertToCubeMap();
    //calculateIrradiance();
    //prefilter();
}

GLuint EnvironmentMap::getTexture() {
    return hdrEquirect;
}

void EnvironmentMap::convertToCubeMap() {
    glGenTextures(1, &envCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
    for (GLuint i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
            SkyboxRes, SkyboxRes, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    cubemapConverter.use();
    cubemapConverter.setInt("equirectangularMap", 0);
    cubemapConverter.setMat4("projectionMatrix", captureCam.projectionMatrix);

    //Render to texture
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, SkyboxRes, SkyboxRes);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    glViewport(0, 0, SkyboxRes, SkyboxRes); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindVertexArray(SkyBox::skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrEquirect);

    for (unsigned int i = 0; i < 6; ++i)
    {
        cubemapConverter.setMat4("viewMatrix", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubeMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, viewportWidth, viewportHeight);
   
}

void EnvironmentMap::calculateIrradiance() {

}

void EnvironmentMap::prefilter() {

}