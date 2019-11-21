#ifndef CONFIGURATION_H
#define CONFIGURATION_H

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
    bool limitFramerate = false;
    bool drawFps = true;
};

extern OptionSet g_options;

#endif
