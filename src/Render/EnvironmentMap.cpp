#include "EnvironmentMap.hpp"

EnvironmentMap::EnvironmentMap() {

}

void EnvironmentMap::loadHDRi(std::string filename) {
    stbi_set_flip_vertically_on_load(true);

    float *data = stbi_loadf((HdrResourcePath + filename).c_str(), &imgWidth, &imgHeight, &nrChannels, 0);
    if (data)
    {
        glGenTextures(1, &hdrEquirect);
        glBindTexture(GL_TEXTURE_2D, hdrEquirect);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, imgWidth, imgHeight, 0, GL_RGB, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load HDR image [" << filename << "." << std::endl;
    }
}

void EnvironmentMap::bind(GLenum tex_unit) {
    glActiveTexture(tex_unit);
    glBindTexture(GL_TEXTURE_2D, hdrEquirect);
}

void EnvironmentMap::process() {

}