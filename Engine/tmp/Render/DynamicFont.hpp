#ifndef DYNAMICFONT_H
#define DYNAMICFONT_H

#include "Engine/Render/Shader.hpp"
#include "Engine/stb_truetype.hpp"

#include "Engine/Core/Logger.hpp"

enum TextAlignment {
    ALIGN_TOP_LEFT = 0,
    ALIGN_MID_LEFT,
    ALIGN_BOT_LEFT,
    ALIGN_TOP_MID,
    ALIGN_MID_MID,
    ALIGN_BOT_MID,
    ALIGN_TOP_RIGHT,
    ALIGN_MID_RIGHT,
    ALIGN_BOT_RIGHT
};

const std::string fontPath = "Data/Fonts/";

using namespace math;
class DynamicFont {
public:
    DynamicFont();

    static const int buffer_size = 1 << 20;
    static unsigned char ttf_buffer[buffer_size];//one megabyte?
                                      //unsigned char temp_bitmap[512*512];

    stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
    GLuint m_ftex;
    float m_fontSize;
    GLuint m_bitmapRes;

    bool initialized = false;

    void create(std::string filename, float fontSize, 
        u32 width, u32 height, int res = 512);
    void resize(u32 width, u32 height);


    GLuint textRectVAO;
    Shader textShader;
    mat4 orthoMat;

    void InitTextRendering();

    void printFontData();
    float getLength(char* text);
    void getTextOffset(float* hOffset, float *vOffset, TextAlignment align, float textLength, float textHeight);
    void drawText(float startX, float startY, math::vec4 color, char *text, TextAlignment align = TextAlignment::ALIGN_TOP_LEFT);
};

#endif
