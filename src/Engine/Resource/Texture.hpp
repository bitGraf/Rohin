#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <iostream>

#include "glad/glad.h"
#include "stb_image.h"
#include "Console.hpp"

const std::string TextureResourcePath = "Data/Images/";

class Texture {
public:
    Texture();

    void loadImage(std::string filename);
    void loadCubemap(std::string filename, std::string filetype);
    void bind(GLenum tex_unit);

    bool trueIfCubemap;

    GLuint glTextureID;

private:
    int width, height, nrChannels;
};

#endif
