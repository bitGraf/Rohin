#ifndef ENVMAP_H
#define ENVMAP_H

#include <string>
#include <iostream>

#include "glad\glad.h"
#include "stb_image.h"

const std::string HdrResourcePath = "Data/Images/";

class EnvironmentMap {
public:
    EnvironmentMap();

    void loadHDRi(std::string filename);
    void bind(GLenum tex_unit);

    void process();

//private:
    int imgWidth, imgHeight, nrChannels;

    GLuint hdrEquirect; //texture of raw image
};

#endif
