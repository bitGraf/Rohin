#include "RenderManager.hpp"

RenderManager::RenderManager() {

}

RenderManager::~RenderManager() {

}

void RenderManager::create() {
    //shaders[ShaderList::mainShader]
    shaders.emplace(ShaderList::mainShader, "mainShader.shader");
    shaders.emplace(ShaderList::shader2,    "shader2.shader");
    shaders.emplace(ShaderList::shader3,    "shader3.shader");
    shaders.emplace(ShaderList::shader4,    "shader4.shader");
}

void RenderManager::destroy() {

}

void RenderManager::setShader(stringID id) {

}