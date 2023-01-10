#ifndef ENTITY_H
#define ENTITY_H

#include <laml/laml.hpp>

/* The idea: we'll see if it works
 * 
 * Lets say an entity is just a uint64, which is supposed to be a unique ID.
 * We don't need 64-bits worth of unique entities, so lets assume 
 * we don't have more than 256 actual entities.
 * 8 bits of that to store an actual ID (256 max ids)
 * the other 56 bits can be used to store 7 8-bit indices into
 * component arrays.
 * 
 * The limits to this:
 * max of 256 entities
 * only 7 kinds of components, and each can only have 256 max
 * 
 * if we instead of 128 bits for the entity id, we can have more.
 * 16 bits for an entity ID => 65,536 max entities
 * if we do 7 component types, then each is 16 bits as well.
 * 
 * Could also have a 12-bit ID => 4096 max entities
 * that leaves 9 component types and an additional 8 bits left over...
 * 
 * How to get 12-bit integer types? or any other non-standard amount?
 * Could use bitsets:
 * 
 * struct entity {
 *   unsigned ID : 12;
 *   unsigned component1 : 12;
 *   ...
 * };
 * and just pack them so that the overall size is what we want.
 * 
 * Gonna experiment with this idea and see where it leads!
 * 
 * Update: doing individual 12-bit bitselts adds padding for each variable to become 16-bits, 
 *         so the entire struct is much larger.
 * 
 * Update: can't make the entire struct just one 128-bit bitset either
 *         could use std::bitset...
 * 
 * Update: instead of a 128-bit bitmask, just use 2 64-bit uints.
 *         can't really do 128-bit constanst like:
 *              0xFFF00000000000000000000000000000
 *         so, don't try and force that
 * 
 * */

// #include <bitset>
// typedef std::bitset < 128 > entity;
/* Map of bits:
 * 
 * first 12: Entity ID
 * every other set of 12: ID in a component
 * Last 8 bits are padding
 * 
 * Bitmasks:
 * 
 * ID: 0xFFF00000000000000000000000000000  12 bits
 * C1: 0x000FFF00000000000000000000000000  12 bits
 * C2: 0x000000FFF00000000000000000000000  12 bits
 * C3: 0x000000000FFF00000000000000000000  12 bits
 * C4: 0x000000000000FFF00000000000000000  12 bits
 * C5: 0x000000000000000FFF00000000000000  12 bits
 * C6: 0x000000000000000000FFF00000000000  12 bits
 * C7: 0x000000000000000000000FFF00000000  12 bits
 * C8: 0x000000000000000000000000FFF00000  12 bits
 * C9: 0x000000000000000000000000000FFF00  12 bits
 * Pd: 0x000000000000000000000000000000FF   8 bits
 * 
 * */
// static_assert(sizeof(entity) == 128/8, "Entity struct is not 128 bits wide!!");

//constexpr std::bitset<128> IDMask 0xFFF00000000000000000000000000000;

/* 128-bits mapped into two 64-bit uints:
 * Can store 10 component IDs, which are indices into tightly 
 * packed component arrays
 * 
 * HighBits:
 * C0: 12 bits  =>  0xFFF0000000000000 => right shift 52 bits?
 * C1: 12 bits  =>  0x000FFF0000000000
 * C2: 12 bits  =>  0x000000FFF0000000
 * C3: 12 bits  =>  0x000000000FFF0000
 * C4: 12 bits  =>  0x000000000000FFF0
 * P1:  4 bits  =>  0x000000000000000F
 * 
 * LowBits:
 * C5: 12 bits  =>  0xFFF0000000000000
 * C6: 12 bits  =>  0x000FFF0000000000
 * C7: 12 bits  =>  0x000000FFF0000000
 * C8: 12 bits  =>  0x000000000FFF0000
 * C9: 12 bits  =>  0x000000000000FFF0
 * P2:  4 bits  =>  0x000000000000000F
 * 
 * */
struct entity {
    uint64 HighBits;
    uint64 LowBits;
};

// Components:
typedef rh::laml::Vec3 position;
typedef rh::laml::Vec3 scale;
typedef rh::laml::Quat orientation;

struct component_tag {
    char* name;
};
struct component_transform {
    position Position;
    scale Scale;
    orientation Orientation;
};
struct component_mesh_render {
    vertex_array_object vao;
};

// the other components
#define COMPONENT_PLACEHOLDER(Number) struct PLACEHOLDER_component_##Number {};
COMPONENT_PLACEHOLDER(3);
COMPONENT_PLACEHOLDER(4);
COMPONENT_PLACEHOLDER(5);
COMPONENT_PLACEHOLDER(6);
COMPONENT_PLACEHOLDER(7);
COMPONENT_PLACEHOLDER(8);
COMPONENT_PLACEHOLDER(9);

template<typename component_type>
struct component_store {
    typedef component_type Type;
    uint16 NumComponents;
    //uint16 Padding[3];
    component_type* Components;
}; // this gets padded to 16 bytes total
static_assert(sizeof(component_store<component_tag>) == 16, "sizeof(component_store) != 10!");

struct entity_registry {
    static const uint16 MaxNumEntities = 4096; // max number storable in 12-bits
    uint16 NumEntities;
    //uint16 Padding[3];
    
    component_store<component_tag> TagComponents;
    component_store<component_transform> TransformComponents;
    component_store<component_mesh_render> MeshRenderComponents;

private: // Just so we don't have to see them 
    component_store<PLACEHOLDER_component_3> TagPlaceholder3;
    component_store<PLACEHOLDER_component_4> TagPlaceholder4;
    component_store<PLACEHOLDER_component_5> TagPlaceholder5;
    component_store<PLACEHOLDER_component_6> TagPlaceholder6;
    component_store<PLACEHOLDER_component_7> TagPlaceholder7;
    component_store<PLACEHOLDER_component_8> TagPlaceholder8;
    component_store<PLACEHOLDER_component_9> TagPlaceholder9;
}; // this gets padded to 168 bytes (10x16 + 2) + 6 bytes of padding
static_assert(sizeof(entity_registry) == 168, "sizeof(entity_registry != 168 bytes!");

// how we would get the ID's of each component
// Could be made more optimale I think by doing 
// them in reverse order and caching the number
// at each step. Instead of shifting left by
// 52, then 40, then 28... each time, make a 
// 64 vit copy and shift it left 4, then read
// then shift it left 12 then read, etc...
// tbh its not that big of a concern anyways...
void func(entity Entity) {
    uint16_t ID = (Entity.HighBits >> 52) & 0xFFF;
    uint16_t C1 = (Entity.HighBits >> 40) & 0xFFF;
    uint16_t C2 = (Entity.HighBits >> 28) & 0xFFF;
    uint16_t C3 = (Entity.HighBits >> 16) & 0xFFF;
    uint16_t C4 = (Entity.HighBits >>  4) & 0xFFF;

    uint16_t C5 = (Entity.LowBits >> 52) & 0xFFF;
    uint16_t C6 = (Entity.LowBits >> 40) & 0xFFF;
    uint16_t C7 = (Entity.LowBits >> 28) & 0xFFF;
    uint16_t C8 = (Entity.LowBits >> 16) & 0xFFF;
    uint16_t C9 = (Entity.LowBits >>  4) & 0xFFF;

    return;
}


#define Component_Type_Tag             0x001
#define Component_Type_Transform       0x002
#define Component_Type_MeshRender      0x004
#define PLACEHOLDER_Component_Type_3   0x008
#define PLACEHOLDER_Component_Type_4   0x010
#define PLACEHOLDER_Component_Type_5   0x020
#define PLACEHOLDER_Component_Type_6   0x040
#define PLACEHOLDER_Component_Type_7   0x080
#define PLACEHOLDER_Component_Type_8   0x100
#define PLACEHOLDER_Component_Type_9   0x200

#define SET_BITS(Value, NewValue, Mask) (Value & ~Mask) | (NewValue & Mask)
entity CreateEntityWithComponents(entity_registry* Registry, uint16 Components) {
    Assert(Registry->NumEntities < Registry->MaxNumEntities);
    Registry->NumEntities++;

    entity NewEntity = {};
    if (Components & Component_Type_Tag) {
        uint64 ID = ((uint64)Registry->TagComponents.NumComponents) << 52;

        NewEntity.HighBits = SET_BITS(NewEntity.HighBits, ID, 0xFFF0000000000000);
        Registry->TagComponents.NumComponents++;
    }
    if (Components & Component_Type_Transform) {
        uint64 ID = ((uint64)Registry->TransformComponents.NumComponents) << 40;

        NewEntity.HighBits = SET_BITS(NewEntity.HighBits, ID, 0x000FFF0000000000);
        Registry->TransformComponents.NumComponents++;
    }
    if (Components & Component_Type_MeshRender) {
        uint64 ID = ((uint64)Registry->MeshRenderComponents.NumComponents) << 28;

        NewEntity.HighBits = SET_BITS(NewEntity.HighBits, ID, 0x000000FFF0000000);
        Registry->MeshRenderComponents.NumComponents++;
    }
    // all the placeholder ones...

    return NewEntity;
}

void EntityTest(entity_registry* Registry) {
    entity Entity1 = CreateEntityWithComponents(Registry, Component_Type_Tag | Component_Type_Transform | Component_Type_MeshRender);
    entity Entity2 = CreateEntityWithComponents(Registry, Component_Type_Tag | Component_Type_MeshRender);
    entity Entity3 = CreateEntityWithComponents(Registry, Component_Type_Tag | Component_Type_Transform | Component_Type_MeshRender);

    func(Entity1);
    func(Entity2);
    func(Entity3);

    return;
}

#endif