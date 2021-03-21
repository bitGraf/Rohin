#ifndef COLLIDER_H
#define COLLIDER_H

#include "Engine/Collision/CollisionHull.hpp"

class Collider {
public:
    Collider();

    CollisionHull* hull;
};

#endif
