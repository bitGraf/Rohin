#pragma once

/* for external use */
#include "Engine/Core/Application.hpp"
#include "Engine/Core/Logger.hpp"
#include "Engine/Core/Timing.hpp"
#include "Engine/Core/GameMath.hpp"

#include "Engine/Core/Input.hpp"
#include "Engine/Core/KeyCodes.hpp"
#include "Engine/Core/MouseButtonCodes.hpp"
#include "Engine/Core/Input.hpp"

#include "Engine/Scene/Scene.hpp"
#include "Engine/GameObject/GameObject.hpp"
#include "Engine/GameObject/Components.hpp"

#include "Engine/EngineLayer.hpp"
#include "Engine/Gui/GuiLayer.hpp"
//#include "Engine/Editor/EditorLayer.hpp"
//#include "Engine/Editor/SceneHierarchyPanel.hpp"

/* Renderer stuff */
#include "Engine/Renderer/RenderCommand.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/TextRenderer.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Buffer.hpp"
#include "Engine/Renderer/VertexArray.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Framebuffer.hpp"
#include "Engine/Renderer/Mesh.hpp"

/* Resource stuff */
#include "Engine/Resources/MeshCatalog.hpp"
#include "Engine/Resources/MaterialCatalog.hpp"
#include "Engine/Resources/nbt/nbt.hpp"

/* Dependencies */
#include "spdlog/spdlog.h"
#include "imgui/imgui.h"