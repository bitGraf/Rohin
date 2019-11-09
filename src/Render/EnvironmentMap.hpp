#ifndef ENVMAP_H
#define ENVMAP_H

#include <string>
#include <iostream>

#include "glad\glad.h"
#include "stb_image.h"

#include "Shader.hpp"
#include "SkyBox.hpp"
#include "Camera.hpp"

const std::string HdrResourcePath = "Data/Images/";

class EnvironmentMap {
public:
    EnvironmentMap();

    void loadHDRi(std::string filename);
    void bindSkybox(GLenum tex_unit);
    void bindIrradiance(GLenum tex_unit);

    void preCompute();

    GLuint getTexture();

private:
    void initShaders();
    //Convert Equirectangular image to cubemap
    void convertToCubeMap();
    //Calaculate IBL Irradiance
    void calculateIrradiance();
    //Prefilter HDR
    void prefilter();

    /* openGL Texture Handles */
    GLuint hdrEquirect; // raw HDRi
    GLuint envCubeMap;  // HDRi mapped to a cube
    GLuint irradianceMap; // Irradiance cubemap

    /* Framebuffer */
    GLuint captureFBO;
    GLuint captureRBO;

    //Resolutions for precomputed textures
    GLuint SkyboxRes;
    GLuint irradianceRes;
    GLuint prefilterRes;
    GLuint brdfLUTRes;

    GLuint viewportWidth;
    GLuint viewportHeight;

    Camera captureCam;
    mat4 captureViews[6];

    /* Shaders for precomputation */
    Shader cubemapConverter;
    Shader irradianceShader;
    Shader prefilterShader;
};

#endif
