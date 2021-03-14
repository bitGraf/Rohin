#pragma once

#include "Engine/Renderer/Shader.hpp"
#include "Engine/Core/Logger.hpp"
#include "Engine/Renderer/Texture.hpp"

typedef struct // copied from <stb_trutype.h>
{
    unsigned short x0, y0, x1, y1; // coordinates of bbox in bitmap
    float xoff, yoff, xadvance;
} _bakedchar;

namespace Engine {

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

    class DynamicFont {
    public:
        void create(std::string filename, float fontSize, int res = 512);
        void printFontData();

        float getLength(const char* text);
        void getTextOffset(float* hOffset, float *vOffset, TextAlignment align, float textLength, float textHeight);

    //private:
        bool initialized = false;
        _bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
        Ref<Texture2D> m_ftex;
        f32 m_fontSize;
        u32 m_bitmapRes;
    };
}
