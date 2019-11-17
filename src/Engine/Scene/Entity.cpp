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

void Entity::parseLevelData(std::istringstream &iss) {
    std::cout << "Default Entity parse" << std::endl;

    std::string entityName = getNextString(iss);
    std::string entityMesh = getNextString(iss);
    std::string entityMat = getNextString(iss);
    math::vec3 entityPos = getNextVec3(iss);
    scalar entityScale = getNextFloat(iss);

    name = entityName;
    setMesh(g_ResourceManager.getMesh(entityMesh));
    setMaterial(g_ResourceManager.getMaterial(entityMat));
    position = entityPos;
    scale = vec3(entityScale);
}