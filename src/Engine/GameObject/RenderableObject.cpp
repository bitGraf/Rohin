#include "RenderableObject.hpp"
const char* RenderableObject::_obj_type_RenderableObject = "Renderable";

RenderableObject::RenderableObject() :
    m_mesh(nullptr),
    m_material(nullptr),
    m_cullRadius(1),
    mesh_Scale(1),
    noCull(true)
{}

void RenderableObject::Create(DataNode* node) {
    GameObject::Create(node);

    m_mesh = ResourceCatalog::GetInstance()->getMesh(
        node->getData("mesh").asString());
    m_material = ResourceCatalog::GetInstance()->getMaterial(
        node->getData("material").asString());
    Position = node->getData("mesh_position").asVec3(vec3());
    YawPitchRoll = node->getData("mesh_yawPitchRoll").asVec3(vec3());
    vec3 _Scale = node->getData("mesh_scale").asVec3(vec3(1));
    m_cullRadius = node->getData("mesh_cullRadius").asFloat(0.25);
}

mat4 RenderableObject::getModelTransform() {
    mat4 objectTransform = GameObject::getTransform();

    return objectTransform * getMeshTransform();
}

void RenderableObject::setModel() {
    ResourceCatalog::GetInstance()->getMesh("Sphere");
    ResourceCatalog::GetInstance()->getMaterial("Woodball");
}

mat4 RenderableObject::getMeshTransform() {
    return (
        mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), vec4(mesh_Position, 1)) *
        mat4(createYawPitchRollMatrix(mesh_YawPitchRoll.x, mesh_YawPitchRoll.y, mesh_YawPitchRoll.z)) * 
        mat4(mesh_Scale.x, mesh_Scale.y, mesh_Scale.z, 1));
}

const char* RenderableObject::ObjectTypeString() {
    return _obj_type_RenderableObject;
}