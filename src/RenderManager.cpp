#include "RenderManager.hpp"

RenderManager::RenderManager() {

}

RenderManager::~RenderManager() {

}

void RenderManager::update(double dt) {

}

void RenderManager::handleMessage(Message msg) {

}

void RenderManager::destroy() {

}

void RenderManager::setShader(stringID id) {
    Message msg;
    msg.type = Message::Type::renderEvent;
    msg.text = std::to_string(id);
    this->putMessage(msg);
}

void RenderManager::sys_create(ConfigurationManager* configMgr) {
    //shaders[ShaderList::mainShader]
    shaders.emplace(ShaderList::mainShader, "mainShader.shader");
    shaders.emplace(ShaderList::shader2,    "shader2.shader");
    shaders.emplace(ShaderList::shader3,    "shader3.shader");
    shaders.emplace(ShaderList::shader4,    "shader4.shader");
}