#pragma once

#include "Engine/Resources/Resource_Types.h"
#include "Engine/Animation/Animation.h"

struct scene_3D;

struct entity_static {
    scene_3D* scene;

    char* name;
    resource_static_mesh*  static_mesh;
    laml::Mat4 transform;
};

struct entity_skinned {
    scene_3D* scene;

    char* name;
    resource_skinned_mesh*  skinned_mesh;
    animation_controller*   controller;
    laml::Mat4 transform;
};

struct scene_3D {
    memory_arena* arena;

    char* name;

    entity_static*  static_entities; // dynarray
    entity_skinned* skinned_entities; // dynarray
};

RHAPI void create_scene(scene_3D* scene, const char* name, memory_arena* arena);

RHAPI entity_static*  create_static_entity(scene_3D* scene, const char* name, resource_static_mesh* mesh);
RHAPI entity_skinned* create_skinned_entity(scene_3D* scene, const char* name, 
                                            resource_skinned_mesh* mesh,
                                            animation_controller* controller);