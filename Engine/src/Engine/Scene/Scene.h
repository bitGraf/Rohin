#pragma once

#include "Engine/Resources/Resource_Types.h"
#include "Engine/Animation/Animation.h"

struct scene_3D;

struct entity_static {
    scene_3D* scene;

    char* name;
    resource_static_mesh*  static_mesh;
    
    laml::Vec3 position;
    laml::Quat orientation;
    laml::Vec3 scale;

    real32 euler_ypr[3];
};

struct entity_skinned {
    scene_3D* scene;

    char* name;
    resource_skinned_mesh*  skinned_mesh;
    animation_controller*   controller;
    
    laml::Vec3 position;
    laml::Quat orientation;
    laml::Vec3 scale;

    real32 euler_ypr[3];
};

struct scene_dir_light {
    laml::Vec3 direction;
    laml::Vec3 color;
    real32 strength;

    bool enabled;
    bool cast_shadow;
    real32 shadowmap_projection_size;
    real32 shadowmap_projection_depth;
    laml::Vec3 origin_point;
    real32 dist_from_origin;
};
struct scene_point_light {
    laml::Vec3 position;
    laml::Vec3 color;
    real32 strength;

    bool cast_shadow;
    bool enabled;
};
struct scene_spot_light {
    laml::Vec3 position;
    laml::Vec3 direction;
    laml::Vec3 color;
    real32 strength;
    real32 inner, outer; // cone angles in degrees!

    bool cast_shadow;
    bool enabled;
};
struct scene_sky_light {
    resource_env_map environment;

    real32 strength;
    bool draw_skybox;
};

struct scene_3D {
    memory_arena* arena;

    char* name;

    entity_static*  static_entities;  // dynarray
    entity_skinned* skinned_entities; // dynarray

    scene_dir_light sun;
    scene_sky_light sky;
    scene_point_light* pointlights; // dynarray
    scene_spot_light*  spotlights;  // dynarray
};

RHAPI void create_scene(scene_3D* scene, const char* name, memory_arena* arena);

RHAPI entity_static*  create_static_entity(scene_3D* scene, const char* name, resource_static_mesh* mesh);
RHAPI entity_skinned* create_skinned_entity(scene_3D* scene, const char* name, 
                                            resource_skinned_mesh* mesh,
                                            animation_controller* controller);
RHAPI bool32 serialize_scene(const char* filename, const scene_3D* scene);