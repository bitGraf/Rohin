#ifndef SKYBOX_H
#define SKYBOX_H

#include <iostream>
#include <string>

#include "glad\glad.h"
#include "stb_image.h"
#include "Resource/Texture.hpp"

#include "GameMath.hpp"

class SkyBox {
public:
    SkyBox();

    void loadFromImages(std::string filename, std::string filetype);
    void loadFromHDRi(std::string path);
    void bind(GLenum texUnit);

    static void InitVAO();

    static GLuint skyboxVAO;
    Texture skyboxTex;
};

#endif
