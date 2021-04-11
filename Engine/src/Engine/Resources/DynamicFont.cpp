#include <enpch.hpp>
#include "DynamicFont.hpp"

#include <stb_truetype.h>

#include "Engine/Resources/MaterialCatalog.hpp"

namespace Engine {

    //unsigned char DynamicFont::ttf_buffer[DynamicFont::buffer_size];
    const int buffer_size = 1 << 20;
    unsigned char ttf_buffer[buffer_size];//one megabyte?
                                      //unsigned char temp_bitmap[512*512];

    void DynamicFont::create(std::string filename, float fontSize, int res) {
        initialized = false;

        ENGINE_LOG_INFO("Loading font: \"{0}\"", filename);
        unsigned char *temp_bitmap = (unsigned char*)malloc(res*res * sizeof(unsigned char));

        FILE* fid = fopen((filename).c_str(), "rb");
        if (fid) {
            size_t bytesRead = fread(ttf_buffer, 1, buffer_size, fid);
            int result = stbtt_BakeFontBitmap(ttf_buffer, 0, fontSize, temp_bitmap, res, res, 32, 96, reinterpret_cast<stbtt_bakedchar*>(cdata)); // no guarantee this fits!
                                                                                                 // can free ttf_buffer at this point

            if (result == -1) {
                ENGINE_LOG_WARN("Failed to create font");
                return;
            }

            m_ftex = MaterialCatalog::GetTexture(temp_bitmap, res);

            //Free bitmap data
            free(temp_bitmap);

            m_bitmapRes = res;
            m_fontSize = fontSize;
            initialized = true;
        }
        else {
            ENGINE_LOG_WARN("Failed to open font file [{0}]\n", filename);
            //exit(1);
        }
    }

    void DynamicFont::printFontData() {
        for (int i = 0; i < 5; i++) {
            printf("x0: %2hu x1: %2hu y0: %2hu y1: %2hu xAdv: %7.3f xoff %6.3f yoff %7.3f\n",
                cdata[i].x0, cdata[i].x1, cdata[i].y0, cdata[i].y1,
                cdata[i].xadvance, cdata[i].xoff, cdata[i].yoff);
        }
    }

    float DynamicFont::getLength(const char* text) {
        float length = 0;
        while (*text && *text != '\n') {
            if (*text >= 32 && *text < 128) {
                length += cdata[*text - 32].xadvance;;
            }
            ++text;
        }
        return length;
    }

    void DynamicFont::getTextOffset(float* hOffset, float *vOffset, TextAlignment align, float textLength, float textHeight) {
        switch (align) {
        case ALIGN_TOP_LEFT: {
            *hOffset = 0;
            *vOffset = textHeight;
        } break;
        case ALIGN_MID_LEFT: {
            *hOffset = 0;
            *vOffset = textHeight / 2;
        } break;
        case ALIGN_BOT_LEFT: {
            *hOffset = 0;
            *vOffset = 0;
        } break;
        case ALIGN_TOP_MID: {
            *hOffset = -textLength / 2;
            *vOffset = textHeight;
        } break;
        case ALIGN_MID_MID: {
            *hOffset = -textLength / 2;
            *vOffset = textHeight / 2;
        } break;
        case ALIGN_BOT_MID: {
            *hOffset = -textLength / 2;
            *vOffset = 0;
        } break;
        case ALIGN_TOP_RIGHT: {
            *hOffset = -textLength;
            *vOffset = textHeight;
        } break;
        case ALIGN_MID_RIGHT: {
            *hOffset = -textLength;
            *vOffset = textHeight / 2;
        } break;
        case ALIGN_BOT_RIGHT: {
            *hOffset = -textLength;
            *vOffset = 0;
        } break;
        default: {
            *hOffset = 0;
            *vOffset = 0;
        } break;
        }
    }
}