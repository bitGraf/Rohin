#ifndef ENTITY_H
#define ENTITY_H

#include "GameMath.hpp"
#include "Models.hpp"

typedef TriangleMesh* meshRef;

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

private:
    meshRef m_mesh;
};

#endif
