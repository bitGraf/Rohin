#ifndef ENTITY_H
#define ENTITY_H

#include <string>

#include "GameMath.hpp"
#include "Render/Models.hpp"
#include "Render/Material.hpp"

class Entity {
public:
    Entity();
    ~Entity();

    math::vec3 position;
    math::mat3 orientation;
    math::vec3 scale;

    void create();
    void update(double dt);
    void render();

    void setMesh(meshRef _mesh);
    void setMaterial(materialRef _mat);

//private:
    meshRef m_mesh;
    materialRef m_material;

    std::string name;
};

#endif
