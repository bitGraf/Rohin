#include "Engine/Core/Timing.h"

#include "Engine/Platform/Platform.h"

time_point start_timer() {
    int64 raw_time = platform_get_wall_clock();

    time_point tp;
    tp.raw_time = raw_time;

    return tp;
}
real64 measure_elapsed_time(time_point start, time_point* new_time) {
    int64 raw_time = platform_get_wall_clock();
    real64 elapsed = platform_get_seconds_elapsed(start.raw_time, raw_time);

    time_point tp;
    tp.raw_time = raw_time;

    if (new_time)
        *new_time = tp;

    return elapsed;
}