#include "SpriteRenderer.hpp"

SpriteRenderer::SpriteRenderer() {
	this->spriteShader = spriteShader;
}

SpriteRenderer::~SpriteRenderer() {
	glDeleteVertexArrays(1, &this->quadVAO);
}

void SpriteRenderer::initRenderData(u32 width, u32 height) {
	Console::logMessage("Initializing Sprite Rendering...");

	orthoMat.orthoProjection(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);

	this->spriteShader.create("sprite.vert", "sprite.frag", "spriteShader");
	this->spriteShader.use();

	Console::logMessage("Initializing sprite VAO");
	// Configure our VAO/VBO
	GLuint vbo;
	GLfloat vertices[] = {
		// Pos      // Tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &this->quadVAO);
	glBindVertexArray(this->quadVAO);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void SpriteRenderer::DrawSprite(Texture &texture, vec2 position, vec2 size,
	GLfloat rotate, vec3 color)
{
	//Prepare transformations
	this->spriteShader.use();
	mat4 model;
	model.translate(vec3(position,0.0f));
	model.translate(vec3(0.5f*size.x, 0.5f*size.y, 0.0f));
	model.rotate(rotate*d2r, vec3(0.0f, 0.0f, 1.0f));
	model.translate(vec3(-0.5f*size.x, -0.5f*size.y, 0.0f));
	model.scale(vec3(size,1.0f));

	this->spriteShader.setMat4("model", model);
	this->spriteShader.setVec3("spriteColor", color);
	this->spriteShader.setInt("image", 0);
	this->spriteShader.setMat4("projection", orthoMat);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.glTextureID);
	glBindVertexArray(this->quadVAO);
	//glCullFace(GL_FRONT);
	glDisable(GL_DEPTH_TEST);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glEnable(GL_DEPTH_TEST);
	//glCullFace(GL_BACK);

	glBindVertexArray(0);
}

void SpriteRenderer::resize(u32 width, u32 height) {
	this->orthoMat.orthoProjection(0, width, height, 0, -1, 1);
}

void SpriteRenderer::refreshShaders() {
	this->spriteShader.create("sprite.vert", "sprite.frag", "spriteShader");
}