#include "RenderableObject.hpp"
#include "Resource\ResourceManager.hpp"

RenderableObject::RenderableObject() :
    m_mesh(nullptr),
    m_material(nullptr),
    m_cullRadius(1),
    mesh_Scale(1)
{
    m_type = GameObjectType::Renderable;
}

void RenderableObject::Create(istringstream &iss, ResourceManager* resource) {
    GameObject::Create(iss, resource);

    m_mesh = resource->getMesh(getNextString(iss));
    m_material = resource->getMaterial(getNextString(iss));
    mesh_Position = getNextVec3(iss);
    mesh_YawPitchRoll = getNextVec3(iss);
    mesh_Scale = getNextVec3(iss);
    m_cullRadius = getNextFloat(iss);
}

mat4 RenderableObject::getModelTransform() {
    mat4 parentTransform = GameObject::getTransform();

    mat4 transform = (
        mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), vec4(Position, 1)) *
        mat4(createYawPitchRollMatrix(YawPitchRoll.x, YawPitchRoll.y, YawPitchRoll.z)));

    return parentTransform * transform;
}

void RenderableObject::setModel(ResourceManager* resource) {
    m_mesh = resource->getMesh("Sphere");
    m_material = resource->getMaterial("Woodball");
}
