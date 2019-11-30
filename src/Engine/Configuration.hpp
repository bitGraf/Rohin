#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "DataTypes.hpp"

static const u16 DEFAULT_SCREEN_WIDTH = 800;
static const u16 DEFAULT_SCREEN_HEIGHT = 600;

// Global Options
class OptionSet {
public:
    // Rendering
    bool wireFrameMode = false;
    bool drawShadows = true;
    bool drawSkybox = true;
    bool drawStaticEntities = true;
    bool drawDynamicEntities = true;
    
    // Scene
    bool updateScene = true;

    // Debug
    bool limitFramerate = true;
    bool highFramerate = false;
    bool drawFps = true;
};

extern OptionSet g_options;

#endif
