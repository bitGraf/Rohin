#include <enpch.hpp>
#include "EngineLayer.hpp"

namespace Engine {
    EngineLayer::EngineLayer(const std::string& name) : m_name(name), m_LayerActive(false) {
    }

    EngineLayer::~EngineLayer() {
    }
}