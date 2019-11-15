#include "EnvironmentMap.hpp"

GLuint EnvironmentMap::skyboxVAO;

EnvironmentMap::EnvironmentMap() {

}

void EnvironmentMap::loadHDRi(std::string filename) {
    stbi_set_flip_vertically_on_load(false);
    alreadyCubemap = false;

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

void EnvironmentMap::loadSkybox(std::string filename, std::string filetype) {
    stbi_set_flip_vertically_on_load(false);
    alreadyCubemap = true;

    //Load each filename
    std::string faces[6] = {
        (filename + "_rt" + filetype).c_str(),
        (filename + "_lf" + filetype).c_str(),
        (filename + "_up" + filetype).c_str(),
        (filename + "_dn" + filetype).c_str(),
        (filename + "_bk" + filetype).c_str(),
        (filename + "_ft" + filetype).c_str()
    };

    //Generate OpenGL Texture
    glGenTextures(1, &envCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);

    for (GLuint i = 0; i < 6; i++) {
        int width, height, nrChannels;
        unsigned char* data = stbi_load((SkyboxResourcePath + faces[i]).c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            printf("Loaded texture: [%dx%d], %d channels\n", width, height, nrChannels);
            stbi_image_free(data);
            SkyboxRes = width;
        }
        else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_set_flip_vertically_on_load(true);
}

void EnvironmentMap::bindSkybox(GLenum tex_unit) {
    glActiveTexture(tex_unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
}

void EnvironmentMap::bindPBR(GLenum tex_unit1, GLenum tex_unit2, GLenum tex_unit3) {
    glActiveTexture(tex_unit1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

    glActiveTexture(tex_unit2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);

    glActiveTexture(tex_unit3);
    glBindTexture(GL_TEXTURE_2D, brdfLUT);
}

void EnvironmentMap::initShaders() {
    cubemapConverter.create("cubemapConvert.vert", "cubemapConvert.frag", "cubemapConverter");
    irradianceShader.create("cubemapConvert.vert", "irradianceCalc.frag", "irradianceShader");
    prefilterShader.create("cubemapConvert.vert", "prefilter.frag", "irradianceShader");
    brdfComputeShader.create("brdf.vert", "brdf.frag", "brdfComputeShader");

    captureCam.m_fov = 90;
    captureCam.m_zFar = 10;
    captureCam.m_zNear = .1;
    captureCam.m_aspectRatio = 1;
    captureCam.updateProjectionMatrix(100, 100);

    captureCam.position = vec3(0.0);
    captureViews[0] = captureCam.lookAt(vec3(0, 0,  1), true).viewMatrix;
    captureViews[1] = captureCam.lookAt(vec3(0, 0, -1), true).viewMatrix;
    captureViews[2] = captureCam.lookAt(vec3(0, -1, 0), true).viewMatrix;
    captureViews[3] = captureCam.lookAt(vec3(0,  1, 0), true).viewMatrix;
    captureViews[4] = captureCam.lookAt(vec3( 1, 0, 0), true).viewMatrix;
    captureViews[5] = captureCam.lookAt(vec3(-1, 0, 0), true).viewMatrix;

    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    //Screen Quad
    float quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
        1.0f, -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 1.0f
    };
    GLuint quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void EnvironmentMap::preCompute() {
    SkyboxRes = 1024;
    irradianceRes = 32;
    prefilterRes = 128;
    brdfLUTRes = 512;

    viewportWidth = 800;
    viewportHeight = 600;

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    auto startTime = std::chrono::system_clock::now();
    initShaders();
    convertToCubeMap();
    calculateIrradiance();
    prefilter();
    computeBRDF();
    auto duration = std::chrono::system_clock::now() - startTime;

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() <<
        " Milliseconds to precompute environment." << std::endl;
}

GLuint EnvironmentMap::getTexture() {
    return hdrEquirect;
}

void EnvironmentMap::convertToCubeMap() {
    if (!alreadyCubemap) {
        glGenTextures(1, &envCubeMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
        for (GLuint i = 0; i < 6; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                SkyboxRes, SkyboxRes, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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
        glBindVertexArray(skyboxVAO);
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

    // generate mip maps
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void EnvironmentMap::calculateIrradiance() {
    //Generate Texture
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0,
            GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Render to Texture
    irradianceShader.use();
    irradianceShader.setInt("environmentMap", 0);
    irradianceShader.setMat4("projectionMatrix", captureCam.projectionMatrix);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, irradianceRes, irradianceRes);

    glViewport(0, 0, irradianceRes, irradianceRes);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);

    for (unsigned int i = 0; i < 6; ++i)
    {
        irradianceShader.setMat4("viewMatrix", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, viewportWidth, viewportHeight);
}

void EnvironmentMap::prefilter() {
    //Generate Texture
    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (GLuint i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
            prefilterRes, prefilterRes, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    //Render to Texture
    prefilterShader.use();
    prefilterShader.setInt("environmentMap", 0);
    prefilterShader.setMat4("projectionMatrix", captureCam.projectionMatrix);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    GLuint maxMipLevels = 5;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
    glBindVertexArray(skyboxVAO);

    for (GLuint mip = 0; mip < maxMipLevels; ++mip) {
        // reisze framebuffer according to mip-level size.
        unsigned int mipWidth = prefilterRes * std::pow(0.5, mip);
        unsigned int mipHeight = prefilterRes * std::pow(0.5, mip);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilterShader.setFloat("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            prefilterShader.setMat4("viewMatrix", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, viewportWidth, viewportHeight);
}

void EnvironmentMap::computeBRDF() {
    glGenTextures(1, &brdfLUT);

    glBindTexture(GL_TEXTURE_2D, brdfLUT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, brdfLUTRes, brdfLUTRes,
        0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
        brdfLUTRes, brdfLUTRes);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, brdfLUT, 0);

    glViewport(0, 0, brdfLUTRes, brdfLUTRes);
    brdfComputeShader.use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, viewportWidth, viewportHeight);

}

void EnvironmentMap::InitVAO() {
    f32 skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };
    GLuint skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices),
        &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
}