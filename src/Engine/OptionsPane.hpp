#ifndef OPTIONS_PANE_H
#define OPTIONS_PANE_H

/*

#include "Configuration.hpp"
#include "Window/Window.hpp"
#include "Render/DynamicFont.hpp"
#include <algorithm>

class OptionsPane {
public:
    OptionsPane();

    void create(Window* mainWin, bool Show = false);
    void redraw();

    void click(s32 key, s32 action);
    void ToggleVisibility();

private:
    bool Visibility;

    struct toggle {
        std::string name;
        bool* value;
        vec4 bounds;
        toggle(std::string s, bool* v, vec4 b) { name = s; value = v; bounds = b; }
    };
    struct toggleSet {
        std::string name;
        std::vector<toggle> settings;

        toggleSet(std::string s, int count, ...) { 
            name = s; 

            va_list args;
            va_start(args, count);

            for (int n = 0; n < count; n++) {
                settings.push_back(va_arg(args, toggle));
            }

            va_end(args);
        }
    };

    std::vector<toggleSet> toggleSettings;

    Window m_pane;
    DynamicFont textFont;
    Window* m_main;

    int xHeader = 5;
    int xSetting = 20;
    float ySpacing;
    float spacing = 1.1;
    vec4 nextBound;
    vec4 getNextBound();
    void nextToggleSet();
    bool inBounds(vec4 bounds, vec2 loc);
};

*/

#endif
