#ifndef COLLISION_WORLD_H
#define COLLISION_WORLD_H

#include "CollisionHull.hpp"
#include "CollisionMath.hpp"

#include <vector>

typedef u64 UID_t;

struct RaycastResult {
    f32 t;
    UID_t colliderID;
    vec3 contactPoint;
    vec3 normal;

    vec3 start, end;
};

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

    void Update(double dt);

    void testCreate(ResourceManager* resource);
    RaycastResult Raycast(vec3 start, vec3 end);
    RaycastResult Raycast(vec3 start, vec3 direction, scalar distance);

//private:
    std::vector<CollisionHull> m_static;
    std::vector<CollisionHull> m_dynamic;
};

extern CollisionWorld cWorld;

#endif
