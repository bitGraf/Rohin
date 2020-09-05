#include "RenderableObject.hpp"
const char* RenderableObject::_obj_type_RenderableObject = "Renderable";

RenderableObject::RenderableObject() :
    m_mesh(nullptr),
    m_material(nullptr),
    m_cullRadius(1),
    mesh_Scale(1),
    noCull(true)
{
    m_type = GameObjectType::Renderable;
}

void RenderableObject::Create(istringstream &iss) {
    GameObject::Create(iss);

    m_mesh = ResourceCatalog::GetInstance()->getMesh(getNextString(iss));
    m_material = ResourceCatalog::GetInstance()->getMaterial(getNextString(iss));
    mesh_Position = getNextVec3(iss);
    mesh_YawPitchRoll = getNextVec3(iss);
    mesh_Scale = getNextVec3(iss);
    m_cullRadius = getNextFloat(iss);
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