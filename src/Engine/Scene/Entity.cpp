#include "Entity.hpp"

Entity::Entity() {
    m_mesh = nullptr;
}

Entity::~Entity() {

}

void Entity::create() {

}

void Entity::update(double dt) {
}

void Entity::render() {

}

void Entity::setMesh(meshRef _mesh) {
    m_mesh = _mesh;
}

void Entity::setMaterial(materialRef _mat) {
    m_material = _mat;  
}

void Entity::parseLevelData(std::istringstream &iss, ResourceManager* resource) {
    //Console::logMessage("Default Entity parse");

    std::string entityName = getNextString(iss);
    std::string entityMesh = getNextString(iss);
    std::string entityMat = getNextString(iss);
    math::vec3 entityPos = getNextVec3(iss);
    math::scalar entityScale = getNextFloat(iss);
    math::scalar entityYaw = getNextFloat(iss);

    name = entityName;
    setMesh(resource->getMesh(entityMesh));
    setMaterial(resource->getMaterial(entityMat));
    position = entityPos;
    scale = math::vec3(entityScale);
    orientation = math::createYawPitchRollMatrix(entityYaw, 0, 0);
}