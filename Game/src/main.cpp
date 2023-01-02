// #include "GameExport.h"

struct rhGame {
    // the game...
};

struct gameExport_t {
    // What the game.dll gives to the engine.exe
    int Version;
    rhGame* Game;
};

struct gameImport_t {
    // Services that engine.exe provides to the game.dll
    int Version;
    // rhSystem* System;
    // rhCommon* Common;
    // ...
};

static rhGame TheGame;
static gameImport_t* TheEngine;

extern "C" gameExport_t* GetGameAPI(gameImport_t* Import) {
    gameExport_t Export = {};
    Export.Version = 1;
    Export.Game = &TheGame;

    if (Import->Version == Export.Version) {
        TheEngine = Import;
        return Export;
    } else {
        return nullptr;
    }
}