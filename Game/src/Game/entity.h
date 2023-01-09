#ifndef ENTITY_H
#define ENTITY_H

#include <laml/laml.hpp>

typedef rh::laml::Vec3 position;
typedef rh::laml::Vec3 scale;
typedef rh::laml::Quat orientation;
typedef uint64 entity_id;

struct transform {
    position Position;
    scale Scale;
    orientation Orientation;
}

#endif