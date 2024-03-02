#pragma once

#include "OpenGL_Types.h"

bool32 OpenGL_create_context();
bool32 OpenGL_destroy_context();

bool32 OpenGL_ImGui_Init();
bool32 OpenGL_ImGui_Shutdown();
bool32 OpenGL_ImGui_Begin_Frame();
bool32 OpenGL_ImGui_End_Frame();
//void OpenGL_swap_buffers();

void OpenGL_set_swap_interval(int32 interval);
int32 OpenGL_get_swap_interval();