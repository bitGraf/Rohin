#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>

#include "Render\Shader.hpp"
#include "stb_truetype.hpp"

#include "Console.hpp"

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

const std::string fontPath = "../../run_tree/Data/Fonts/";

using namespace math;
class DynamicFont {
public:
    DynamicFont();

    static const int buffer_size = 1 << 20;
    unsigned char ttf_buffer[buffer_size];//one megabyte?
                                      //unsigned char temp_bitmap[512*512];

    stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
    GLuint m_ftex;
    float m_fontSize;
    GLuint m_bitmapRes;

    bool initialized = false;
    bool p = true;

    void create(std::string filename, float fontSize, int res = 512);



    static GLuint textRectVAO;
    static Shader textShader;
    static mat4 orthoMat;

    static void InitTextRendering(u32 width, u32 height);

    void printFontData();
    float getLength(char* text);
    void getTextOffset(float* hOffset, float *vOffset, TextAlignment align, float textLength, float textHeight);
    void drawText(float startX, float startY, math::vec4 color, char *text, TextAlignment align = TextAlignment::ALIGN_TOP_LEFT);
};

#endif
