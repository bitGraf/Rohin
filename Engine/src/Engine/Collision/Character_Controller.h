#pragma once

#include "Engine/Collision/Collision_Types.h"

//enum controller_type : uint32 {
//    sphere,
//    capsule
//};
//struct character_controller {
//    laml::Vec3 position;
//    laml::Quat orientation;
//
//    laml::Vec3 collider_offset;
//    laml::Vec3 up_dir;
//    real32 radius;
//    real32 height;
//
//    real32 pitch;
//    real32 yaw;
//};

RHAPI laml::Vec3 collision_move_sphere(laml::Vec3 start, laml::Vec3 end, real32 radius, collision_triangle* triangles);