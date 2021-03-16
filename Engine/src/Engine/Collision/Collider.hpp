#ifndef COLLIDER_H
#define COLLIDER_H

#include "CollisionHull.hpp"

class Collider {
public:
    Collider();

    CollisionHull* hull;
};

#endif
