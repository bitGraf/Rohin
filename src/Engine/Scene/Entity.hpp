#ifndef ENTITY_H
#define ENTITY_H

#include <string>

#include "GameMath.hpp"
#include "Models.hpp"
#include "Material.hpp"
#include "Editor\Pickable.hpp"

class Entity : public Pickable {
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
