#ifndef COLLISION_WORLD_H
#define COLLISION_WORLD_H

#include "CollisionHull.hpp"
#include "CollisionMath.hpp"

#include <vector>

typedef u64 UID_t;

struct ShapeCastContact {
    f32 t;
    UID_t colliderID;
    vec3 contactPoint;
    vec3 normal;
};

#define MAX_CONTACTS 8

struct ShapeCastResult {
    u8 numContacts;
    ShapeCastContact contacts[MAX_CONTACTS];
};

class CollisionWorld {
public:
    CollisionWorld();

    void step(double dt);

    vec3 getVelocity(CollisionHull* hull, vec3 wishVel);

    void shapeCast(CollisionHull* shape, vec3 start, vec3 dir, ShapeCastResult *res);

private:
    std::vector<CollisionHull> m_hulls;
};

extern CollisionWorld cWorld;

#endif
