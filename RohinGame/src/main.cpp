#include "rohin_game.h"

#include <Engine/Application.h>
bool32 create_application(RohinApp* app);
#include <Engine/Entry_Point.h>

bool32 create_application(RohinApp* app) {
    app->app_config.application_name = "Rohin Game";
    
    //app->app_config.start_x = 540;
    //app->app_config.start_y = 100;
    //app->app_config.start_width = 1280;
    //app->app_config.start_height = 720;
    app->app_config.start_x = 10;
    app->app_config.start_y = 50;
    app->app_config.start_width = 960;
    app->app_config.start_height = 720;

    app->app_config.requested_permanant_memory = Megabytes(32);
    app->app_config.requested_transient_memory = Megabytes(256);

    app->startup = game_startup;
    app->initialize = game_initialize;
    app->update_and_render = game_update_and_render;
    app->on_resize = game_on_resize;
    app->shutdown = game_shutdown;

    return true;
}