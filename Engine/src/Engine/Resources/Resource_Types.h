#pragma once

#include "Engine/Renderer/Render_Types.h"

struct static_mesh {
    laml::Mat4 model_transform;
    triangle_geometry geometry;
};

#if 0
struct anim_mesh {
    laml::Mat4 model_transform;
    triangle_geometry geometry;
};

enum mesh_type {
    mesh_type_static,
    mesh_type_animated
};

struct model {
    mesh_type Type;
    union {
        static_mesh Mesh;
        anim_mesh AnimMesh;
    };
};
#endif