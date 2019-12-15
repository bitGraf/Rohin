#include "GameObject.hpp"
#include "Scene/\Scene.hpp"

const char* GameObject::_obj_type_GameObject = "GameObject";

GameObject::GameObject() :
    Position(),
    YawPitchRoll(),
    m_uid(getNextUID()),
    m_debugMesh(nullptr),
    m_type(GameObjectType::NONE),
    m_parent(nullptr)
{
    bool k = false;
}

void GameObject::Create(istringstream &iss, ResourceManager* resource) {
    Name = getNextString(iss);
    Position = getNextVec3(iss);
    YawPitchRoll = getNextVec3(iss);
    vec3 _Scale = getNextVec3(iss);

    string parentName = getNextString(iss);
    if (parentName.compare("") != 0) {
        /// Move this to PostLoad()
        //m_parent = GetScene()->getObjectByName(parentName);
    }

    Console::logMessage("GameObject: %llu {%s} created.", (m_uid) , Name.c_str());
}

void GameObject::Update(double dt) {}
void GameObject::Destroy() {}
void GameObject::PostLoad() {}
void GameObject::InputEvent(Message::Datatype key, Message::Datatype action) {}
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

    if (m_parent) {
        // This will recursively apply all of the parent's transformations
        return m_parent->getTransform() * transform;
    }
    else {
        return transform;
    }
}


UID_t GameObject::nextUID = 0;
UID_t GameObject::getNextUID() {
    return ++nextUID;
}