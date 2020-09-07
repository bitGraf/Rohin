#include "GameObject.hpp"
#include "Scene/Scene.hpp"

const char* GameObject::_obj_type_GameObject = "GameObject";

GameObject::GameObject() :
    Position(),
    YawPitchRoll(),
    m_uid(getNextUID()),
    m_debugMesh(nullptr)
{
    bool k = false;
}

void GameObject::Create(DataNode* node) {
    Name = node->getData("name").asString();
    Position = node->getVec3("posx", "posy", "posz");
    YawPitchRoll = node->getVec3("yaw", "pitch", "roll");
    vec3 _Scale = node->getVec3("scalex", "scaley", "scalez");

    Console::logMessage("GameObject: %llu {%s} created.", (m_uid) , Name.c_str());
}

void GameObject::Update(double dt) {}
void GameObject::Destroy() {}
void GameObject::PostLoad() {}
void GameObject::InputEvent(s32 key, s32 action) {}
const char* GameObject::ObjectTypeString() {
    return _obj_type_GameObject;
}

UID_t GameObject::getID() const {
    return m_uid;
}

mat4 GameObject::getTransform() const {
    mat4 transform = (
        mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), vec4(Position, 1)) *
        mat4(createYawPitchRollMatrix(YawPitchRoll.x, YawPitchRoll.y, YawPitchRoll.z)));

    return transform;
}


UID_t GameObject::nextUID = 0;
UID_t GameObject::getNextUID() {
    return ++nextUID;
}