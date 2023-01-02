#pragma once

#include "Engine/Core/Base.hpp"

namespace rh {
    class Scene3D;
    class GameObject;

    bool LoadExampleLevel(const std::string& levelName, Scene3D* scene);
}
