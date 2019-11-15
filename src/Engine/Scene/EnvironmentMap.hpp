#ifndef ENVMAP_H
#define ENVMAP_H

#include <string>
#include <iostream>
#include <chrono>

#include "glad\glad.h"
#include "stb_image.h"

#include "Render/Shader.hpp"
#include "Camera.hpp"

const std::string SkyboxResourcePath = "Data/Images/";
const std::string HdrResourcePath = "Data/Images/";

class EnvironmentMap {
public:
    EnvironmentMap();

    bool alreadyCubemap;

    void loadSkybox(std::string filename, std::string filetype);
    void loadHDRi(std::string filename);
    void bindSkybox(GLenum tex_unit);
    void bindPBR(GLenum tex_unit1, GLenum tex_unit2, GLenum tex_unit3);

    void preCompute();
    GLuint getTexture();

    static void InitVAO();
    static GLuint skyboxVAO;

private:
    void initShaders();
    //Convert Equirectangular image to cubemap
    void convertToCubeMap();
    //Calaculate IBL Irradiance
    void calculateIrradiance();
    //Prefilter HDR
    void prefilter();
    //Compute BRDF
    void computeBRDF();

    /* openGL Texture Handles */
    GLuint hdrEquirect; // raw HDRi
    GLuint envCubeMap;  // HDRi mapped to a cube
    GLuint irradianceMap; // Irradiance cubemap
    GLuint prefilterMap;  // Prefilterd specular
    GLuint brdfLUT;     // Precomputed BRDF integral

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
    Shader brdfComputeShader;

    GLuint quadVAO;    
};

#endif
