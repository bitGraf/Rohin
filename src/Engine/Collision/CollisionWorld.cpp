#include "CollisionWorld.hpp"

CollisionWorld cWorld;

CollisionWorld::CollisionWorld() {

}

void CollisionWorld::step(double dt) {

}

vec3 CollisionWorld::getVelocity(CollisionHull* hull, vec3 wishVel) {
    return wishVel;
}

void CollisionWorld::shapeCast(CollisionHull* shape, vec3 start, vec3 dir, ShapeCastResult *res) {

}