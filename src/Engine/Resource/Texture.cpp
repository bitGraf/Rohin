#include "Texture.hpp"

Texture::Texture() {}

void Texture::loadImage(std::string filename) {
    trueIfCubemap = false;
    stbi_set_flip_vertically_on_load(true);

    //Generate OpenGL Texture
    glGenTextures(1, &glTextureID);
    glBindTexture(GL_TEXTURE_2D, glTextureID);
    // set the texture wrapping/filtering options

    //Load image data
    unsigned char *data = stbi_load((TextureResourcePath + filename).c_str(),
        &width, &height, &nrChannels, 0);

    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        std::cout << "Failed to load texture [" << filename << "]" << std::endl;
    }

    stbi_image_free(data);
}

void Texture::loadCubemap(std::string filename, std::string filetype) {
    trueIfCubemap = true;
    stbi_set_flip_vertically_on_load(false);

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
    glGenTextures(1, &glTextureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, glTextureID);

    for (GLuint i = 0; i < 6; i++) {
        unsigned char* data = stbi_load((TextureResourcePath + faces[i]).c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            printf("Loaded texture: [%dx%d], %d channels\n", width, height, nrChannels);
            stbi_image_free(data);
        }
        else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    stbi_set_flip_vertically_on_load(true);
}

void Texture::bind(GLenum tex_unit) {
    glActiveTexture(tex_unit);
    glBindTexture(trueIfCubemap ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, glTextureID);
}