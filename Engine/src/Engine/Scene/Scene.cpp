#include "Scene.h"

#include "Engine/Memory/Memory.h"
#include "Engine/Memory/Memory_Arena.h"
#include "Engine/Core/String.h"

void create_scene(scene_3D* scene, const char* name, memory_arena* arena) {
    scene->name = copy_string_to_arena(name, arena);
    scene->arena = arena;

    // define default lights
    scene->sun.direction = laml::Vec3(0.0f, -1.0f, 0.0f);
    scene->sun.color = laml::rgb8_to_rgba32f(201, 226, 255);
    scene->sun.strength = 1.0f;

    scene->pointlights = CreateArray(arena, scene_point_light, 10);
    scene->spotlights  = CreateArray(arena, scene_spot_light,  10);

    scene->sky.placeholder = 0; // placeholder

    // reserve dynarrays for entity types
    scene->static_entities  = CreateArray(arena, entity_static,  10);
    scene->skinned_entities = CreateArray(arena, entity_skinned, 10);
}


entity_static* create_static_entity(scene_3D* scene, const char* name, resource_static_mesh* mesh) {
    entity_static entity;
    entity.scene = scene;
    entity.name = copy_string_to_arena(name, scene->arena);
    entity.static_mesh = mesh;

    entity.position    = laml::Vec3(0.0f, 0.0f, 0.0f);
    entity.orientation = laml::Quat(0.0f, 0.0f, 0.0f, 1.0f);
    entity.scale       = laml::Vec3(1.0f, 1.0f, 1.0f);

    ArrayPushPtr(scene->static_entities, &entity, sizeof(entity_static));
    entity_static* new_entity_ptr = ArrayPeek(scene->static_entities);

    return new_entity_ptr;
}

entity_skinned* create_skinned_entity(scene_3D* scene, const char* name, 
                                      resource_skinned_mesh* mesh,
                                      animation_controller* controller) {
    entity_skinned entity;
    entity.scene = scene;
    entity.name = copy_string_to_arena(name, scene->arena);
    entity.skinned_mesh = mesh;
    entity.controller = controller;
    
    entity.position    = laml::Vec3(0.0f, 0.0f, 0.0f);
    entity.orientation = laml::Quat(0.0f, 0.0f, 0.0f, 1.0f);
    entity.scale       = laml::Vec3(1.0f, 1.0f, 1.0f);

    ArrayPushPtr(scene->skinned_entities, &entity, sizeof(entity_skinned));
    entity_skinned* new_entity_ptr = ArrayPeek(scene->skinned_entities);

    return new_entity_ptr;
}

bool32 serialize_scene(const char* filename, const scene_3D* scene) {
    return false;
}