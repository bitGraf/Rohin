#include "Scene/Scene.hpp"
#include "Engine.hpp"

void GlobalHandleMessage(Message msg);
Engine g_engine;

int main(int argc, char* argv[]) {
	g_engine.InitEngine(GlobalHandleMessage, argc, argv);
	g_engine.LoadLevel("Data/outFile.scene");

	g_engine.Start();

	Shader shader;
	shader.create("sprite.vert", "sprite.frag", "spriteRendererShader");
	SpriteRenderer testRender();
	//system("pause");

	return 0;
}

void GlobalHandleMessage(Message msg) {
	g_engine.globalHandle(msg);
}
