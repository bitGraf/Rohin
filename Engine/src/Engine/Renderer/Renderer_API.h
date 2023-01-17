#pragma once

#include "Render_Types.h"

struct platform_state;
struct memory_arena;

bool32 renderer_api_create(memory_arena* arena, renderer_api_type type, platform_state* plat_state, renderer_api** out_api);
void renderer_api_destroy(renderer_api* api);