#include "SpriteRenderer.hpp"

SpriteRenderer::SpriteRenderer() {

}

SpriteRenderer::~SpriteRenderer()
{
	glDeleteVertexArrays(1, &this->quadVAO);
}

void SpriteRenderer::initRenderData()
{
	Console::logMessage("Initializing Text Rendering...");

	spriteShader.create("sprite.vert", "sprite.frag", "spriteShader");
	spriteShader.use();
	//spriteShader.setInt("fontTex", 0);

	Console::logMessage("Initializing sprite VAO");
	// Configure our VAO/VBO
	GLuint VBO;
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
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(this->quadVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), (GLvoid*)0);
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
	model.rotate(rotate, vec3(0.0f, 0.0f, 1.0f));
	model.translate(vec3(-0.5f*size.x, -0.5f*size.y, 0.0f));
	model.scale(vec3(size,1.0f));

	this->spriteShader.setMat4("model", model);
	this->spriteShader.setVec3("spriteColor", color);

	texture.bind(GL_TEXTURE0);

	glBindVertexArray(this->quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}