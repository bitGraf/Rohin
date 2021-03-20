#pragma once

namespace Engine {

    class SoundEngine {
    public:

        static void Init();
        static void StartSource();
        static void Update(double dt);
        static void Shutdown();
    };
}