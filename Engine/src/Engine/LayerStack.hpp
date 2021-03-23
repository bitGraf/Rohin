#pragma once

#include "Engine/core/base.hpp"
#include "Engine/EngineLayer.hpp"

#include <vector>

namespace Engine {

    class LayerStack
    {
    public:
        LayerStack();
        ~LayerStack();

        void PushLayer(EngineLayer* layer);
        void PushOverlay(EngineLayer* overlay);
        void PopLayer(EngineLayer* layer);
        void PopOverlay(EngineLayer* overlay);

        std::vector<EngineLayer*>::iterator begin() { return m_layers.begin(); }
        std::vector<EngineLayer*>::iterator end() { return m_layers.end(); }

        std::vector<EngineLayer*>::reverse_iterator rbegin() { return m_layers.rbegin(); }
        std::vector<EngineLayer*>::reverse_iterator rend() { return m_layers.rend(); }

    private:
        std::vector<EngineLayer*> m_layers;
       u32 m_layerInsertIndex;
    };

}