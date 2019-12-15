#include "DynamicFont.hpp"

//GLuint DynamicFont::textRectVAO = 0;
//Shader DynamicFont::textShader;
//mat4 DynamicFont::orthoMat;
unsigned char DynamicFont::ttf_buffer[DynamicFont::buffer_size];

DynamicFont::DynamicFont() {

}

void DynamicFont::InitTextRendering() {
    Console::logMessage("Initializing Text Rendering...");

    textShader.create("text.vert", "text.frag", "textShader");
    textShader.use();
    textShader.setInt("fontTex", 0);
    
    Console::logMessage("Initializing text rendering VAO");
    GLuint vbo;

    float quadVertices[] = {//vec2 position, vec2 uv
        0, 1,
        0, 0,
        1, 0,

        0, 1,
        1, 0,
        1, 1
    };

    glGenVertexArrays(1, &textRectVAO);
    glBindVertexArray(textRectVAO);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    //glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void DynamicFont::create(std::string filename, float fontSize,
        u32 width, u32 height, int res) {
    initialized = false;

    orthoMat.orthoProjection(0, width, height, 0, -1, 1);

    //printf("Loading font: \"%s\"\n", filename);
    unsigned char *temp_bitmap = (unsigned char*)malloc(res*res * sizeof(unsigned char));

    FILE* fid = fopen((fontPath+filename).c_str(), "rb");
    if (fid) {
        size_t bytesRead = fread(ttf_buffer, 1, buffer_size, fid);
        int result = stbtt_BakeFontBitmap(ttf_buffer, 0, fontSize, temp_bitmap, res, res, 32, 96, cdata); // no guarantee this fits!
                                                                                             // can free ttf_buffer at this point

        if (result == -1) {
            Console::logMessage("Failed to create font");
            return;
        }
        glGenTextures(1, &m_ftex);
        glBindTexture(GL_TEXTURE_2D, m_ftex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, res, res, 0, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap);
        // can free temp_bitmap at this point
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        //Free bitmap data
        free(temp_bitmap);

        m_bitmapRes = res;
        m_fontSize = fontSize;
        initialized = true;
    } else {
        printf("Failed to open font file [%s]\n", filename.c_str());
        //exit(1);
    }
}

void DynamicFont::resize(u32 width, u32 height) {
    orthoMat.orthoProjection(0, width, height, 0, -1, 1);
}

void DynamicFont::printFontData() {
    for (int i = 0; i < 5; i++) {
        printf("x0: %2hu x1: %2hu y0: %2hu y1: %2hu xAdv: %7.3f xoff %6.3f yoff %7.3f\n",
            cdata[i].x0, cdata[i].x1, cdata[i].y0, cdata[i].y1,
            cdata[i].xadvance, cdata[i].xoff, cdata[i].yoff);
    }
}

float DynamicFont::getLength(char* text) {
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

void DynamicFont::drawText(float startX, float startY, math::vec4 color, char *text, TextAlignment align) {
    if (initialized) {
        float x = startX;
        float y = startY;

        float hOff, vOff;
        getTextOffset(&hOff, &vOff, align, getLength(text), m_fontSize);

        // assume orthographic projection with units = screen pixels, origin at top left
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_ftex);
        glBindVertexArray(textRectVAO);
        glCullFace(GL_FRONT);
        glDisable(GL_DEPTH_TEST);
        textShader.use();

        textShader.setVec4("textColor", color);
        textShader.setMat4("projectionMatrix", orthoMat);

        while (*text) {
            if (*text == '\n') {
                //Increase y by one line,
                //reset x to start
                x = startX;
                y += m_fontSize;
            }
            if (*text >= 32 && *text < 128) {
                stbtt_aligned_quad q;
                char c = *text - 32;
                stbtt_GetBakedQuad(cdata, m_bitmapRes, m_bitmapRes, *text - 32, &x, &y, &q, 1);//1=opengl & d3d10+,0=d3d9

                float scaleX = q.x1 - q.x0;
                float scaleY = q.y1 - q.y0;
                float transX = q.x0;
                float transY = q.y0;
                textShader.setVec4("transform", math::vec4(scaleX, scaleY, transX + hOff, transY + vOff));

                scaleX = q.s1 - q.s0;
                scaleY = q.t1 - q.t0;
                transX = q.s0;
                transY = q.t0;
                textShader.setVec4("transformUV", math::vec4(scaleX, scaleY, transX, transY));

                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            ++text;
        }
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_BACK);
        glBindVertexArray(0);
    }
}