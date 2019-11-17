#ifndef STATIC_ENTITY_H
#define STATIC_ENTITY_H

#include "Scene\Entity.hpp"

class StaticEntity : public Entity {
public:
    StaticEntity();

    void parseLevelData(std::istringstream &iss);
    virtual void func();
};

#endif