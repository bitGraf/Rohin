#ifndef PUPPET_H
#define PUPPET_H

#include "Scene\Entity.hpp"
#include "GameMath.hpp"

using namespace math;

class Puppet : public Entity {
public: 
    Puppet();

    void create();
    void update(double dt);
    void render();
    void parseLevelData(std::istringstream &iss, ResourceManager* resource);

    void move(vec3 wishDir);

private:
    vec3 floor;
};

#endif
