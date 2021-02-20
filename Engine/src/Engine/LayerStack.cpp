#include "enpch.hpp"
#include "LayerStack.hpp"

namespace Engine {
    LayerStack::LayerStack() : m_layerInsertIndex(0) {
    }

    LayerStack::~LayerStack() {
        for (auto layer : m_layers) {
            layer->OnDetach();
            delete layer;
        }
    }

    void LayerStack::PushLayer(EngineLayer* layer) {
        m_layers.emplace(m_layers.begin() + m_layerInsertIndex, layer);
        m_layerInsertIndex++;
    }

    void LayerStack::PushOverlay(EngineLayer* overlay) {
        m_layers.emplace_back(overlay);
    }

    void LayerStack::PopLayer(EngineLayer* layer) {
        auto it = std::find(m_layers.begin(), m_layers.end(), layer);
        if (it != m_layers.end()) {
            layer->OnDetach();
            m_layers.erase(it);
            m_layerInsertIndex--;
        }
    }

    void LayerStack::PopOverlay(EngineLayer* overlay) {
        auto it = std::find(m_layers.begin(), m_layers.end(), overlay);
        if (it != m_layers.end()) {
            overlay->OnDetach();
            m_layers.erase(it);
        }
    }
}