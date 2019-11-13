#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <iostream>

#include "glad\glad.h"
#include "stb_image.h"

const std::string TextureResourcePath = "../../run_tree/Data/Images/";

class Texture {
public:
    Texture();

    void loadImage(std::string filename);
    void loadCubemap(std::string filename, std::string filetype);
    void bind(GLenum tex_unit);

    bool trueIfCubemap;

private:
    int width, height, nrChannels;
    GLuint glTextureID;
};

#endif
