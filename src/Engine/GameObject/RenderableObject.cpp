#include "RenderableObject.hpp"
const char* RenderableObject::_obj_type_RenderableObject = "Renderable";

RenderableObject::RenderableObject() :
    m_mesh(nullptr),
    m_material(nullptr),
    m_cullRadius(1),
    mesh_Scale(1),
    noCull(true)
{}

void RenderableObject::Create(jsonObj node) {
    GameObject::Create(node);

    m_mesh = ResourceCatalog::GetInstance()->getMesh(
        safeAccess<std::string>(node, "mesh", "_meshName_"));
    m_material = ResourceCatalog::GetInstance()->getMaterial(
        safeAccess<std::string>(node, "material", "_materialName_"));

    Position = safeAccessVec<vec3>(node, "mesh_position", vec3());
    YawPitchRoll = safeAccessVec<vec3>(node, "mesh_yawPitchRoll", vec3());
    vec3 _Scale = safeAccessVec<vec3>(node, "mesh_scale", vec3());
    m_cullRadius = safeAccess<double>(node, "mesh_cullRadius", 1.0);
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