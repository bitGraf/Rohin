#pragma once

#include <Engine/Defines.h>
#include <Engine/Application.h>

// function pointers to game code
bool32 game_startup(RohinApp* app);
bool32 game_initialize(RohinApp* app);
bool32 game_update_and_render(RohinApp* app, render_packet* packet, real32 delta_time);
void game_on_resize(RohinApp* app, uint32 new_width, uint32 new_height);
void game_shutdown(RohinApp* app);