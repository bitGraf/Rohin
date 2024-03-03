#pragma once

#include "Engine/Defines.h"

struct time_point {
    int64 raw_time;
};

RHAPI time_point start_timer();
RHAPI real64 measure_elapsed_time(time_point start, time_point* new_time = nullptr);