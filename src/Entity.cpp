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