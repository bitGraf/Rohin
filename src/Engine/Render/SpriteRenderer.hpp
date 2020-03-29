#ifndef SPRITE_H
#define SPRITE_H

#include <stdio.h>
#include <stdlib.h>

#include "Render/Shader.hpp"
#include "stb_truetype.hpp"

#include "Console.hpp"
#include "Resource/Texture.hpp"

using namespace math;
class SpriteRenderer
{
public:
	SpriteRenderer();
	~SpriteRenderer();

	void DrawSprite(Texture &texture, vec2 position, vec2 size = vec2(10, 10),
		GLfloat rotate = 0.0f, vec3 color = vec3(1.0f));
	void initRenderData();

private:
	Shader spriteShader;
	GLuint quadVAO;
};

#endif