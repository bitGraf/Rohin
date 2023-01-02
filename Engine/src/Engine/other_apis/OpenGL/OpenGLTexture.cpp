#include <enpch.hpp>
#include "OpenGLTexture.hpp"

#include <glad/glad.h>
#include <stb_image.h>

namespace rh {

    /* Texture2D *******************************************/
    OpenGLTexture2D::OpenGLTexture2D(const std::string& path) 
        : m_Path(path) {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);
        stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        ENGINE_LOG_ASSERT(data, "Failed to load image file");
        m_Width = width;
        m_Height = height;

        glGenTextures(1, &m_TextureID);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);

        GLenum internalFormat = 0, format = 0;

        if (channels == 4) {
            internalFormat = GL_RGBA8;
            format = GL_RGBA;
        } else if (channels == 3) {
            internalFormat = GL_RGB8;
            format = GL_RGB;
        }
        else if (channels == 2) {
            internalFormat = GL_RG8;
            format = GL_RG;
        }
        else if (channels == 1) {
            internalFormat = GL_R8;
            format = GL_RED;
        }

        ENGINE_LOG_ASSERT(internalFormat & format, "Unsupported image format");

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        float aniso = 0.0f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &aniso);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, aniso);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }

    OpenGLTexture2D::OpenGLTexture2D(const unsigned char* bitmap, u32 res) {
        ENGINE_LOG_ASSERT(bitmap, "Bad data passed to OpenGLTexture2D");
        m_Width = res;
        m_Height = res;

        glGenTextures(1, &m_TextureID);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, res, res, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        /*
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        */
    }
    
    OpenGLTexture2D::~OpenGLTexture2D() {
        glDeleteTextures(1, &m_TextureID);
    }

    void OpenGLTexture2D::Bind(u32 slot) const {
        glActiveTexture(GL_TEXTURE0+slot);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
    }


    /* TextureCube ****************************************************************************/

    // load from a 'cube-cross' layout
    OpenGLTextureCube::OpenGLTextureCube(const std::string& path)
        : m_Path(path) {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(false);
        stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb);

        m_Width = width;
        m_Height = height;
        GLenum format = GL_RGB;
        GLenum internalFormat = GL_RGB8;

        u32 faceWidth = m_Width / 4;
        u32 faceHeight = m_Height / 3;
        ENGINE_LOG_ASSERT(faceWidth == faceHeight, "Non-square faces!");

        std::array<u8*, 6> faces;
        for (size_t i = 0; i < faces.size(); i++)
            faces[i] = new u8[faceWidth * faceHeight * 3]; // 3 BPP

        int faceIndex = 0;

        for (size_t i = 0; i < 4; i++)
        {
            for (size_t y = 0; y < faceHeight; y++)
            {
                size_t yOffset = y + faceHeight;
                for (size_t x = 0; x < faceWidth; x++)
                {
                    size_t xOffset = x + i * faceWidth;
                    faces[faceIndex][(x + y * faceWidth) * 3 + 0] = data[(xOffset + yOffset * m_Width) * 3 + 0];
                    faces[faceIndex][(x + y * faceWidth) * 3 + 1] = data[(xOffset + yOffset * m_Width) * 3 + 1];
                    faces[faceIndex][(x + y * faceWidth) * 3 + 2] = data[(xOffset + yOffset * m_Width) * 3 + 2];
                }
            }
            faceIndex++;
        }

        for (size_t i = 0; i < 3; i++)
        {
            // Skip the middle one
            if (i == 1)
                continue;

            for (size_t y = 0; y < faceHeight; y++)
            {
                size_t yOffset = y + i * faceHeight;
                for (size_t x = 0; x < faceWidth; x++)
                {
                    size_t xOffset = x + faceWidth;
                    faces[faceIndex][(x + y * faceWidth) * 3 + 0] = data[(xOffset + yOffset * m_Width) * 3 + 0];
                    faces[faceIndex][(x + y * faceWidth) * 3 + 1] = data[(xOffset + yOffset * m_Width) * 3 + 1];
                    faces[faceIndex][(x + y * faceWidth) * 3 + 2] = data[(xOffset + yOffset * m_Width) * 3 + 2];
                }
            }
            faceIndex++;
        }

        glGenTextures(1, &m_TextureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        float maxAnisotropy; 
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);
        glTextureParameterf(m_TextureID, GL_TEXTURE_MAX_ANISOTROPY, maxAnisotropy);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[2]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[0]);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[4]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[5]);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[1]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[3]);

        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        glBindTexture(GL_TEXTURE_2D, 0);

        for (size_t i = 0; i < faces.size(); i++)
            delete[] faces[i];

        stbi_image_free(data);
    }

    OpenGLTextureCube::~OpenGLTextureCube() {
        glDeleteTextures(1, &m_TextureID);
    }

    void OpenGLTextureCube::Bind(u32 slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);
    }
}
