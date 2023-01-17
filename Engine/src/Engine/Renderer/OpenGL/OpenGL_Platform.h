#pragma once

#include "OpenGL_Types.h"

bool32 OpenGL_create_context();
//void OpenGL_swap_buffers();

void OpenGL_set_swap_interval(int32 interval);
int32 OpenGL_get_swap_interval();