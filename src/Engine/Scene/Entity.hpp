#ifndef ENTITY_H
#define ENTITY_H

#include <string>

#include "Configuration.hpp"

#include "GameMath.hpp"
#include "Models.hpp"
#include "Material.hpp"
#include "Utils.hpp"
#include "Resource\ResourceManager.hpp"

class Entity {
public:
    Entity();
    ~Entity();

    math::vec3 position;
    math::mat3 orientation;
    math::vec3 scale;

    virtual void create();
    virtual void update(double dt);
    virtual void render();
    virtual void parseLevelData(std::istringstream &iss, ResourceManager* resource);

    void setMesh(meshRef _mesh);
    void setMaterial(materialRef _mat);

//private:
    meshRef m_mesh;
    materialRef m_material;

    std::string name;
};

#endif
