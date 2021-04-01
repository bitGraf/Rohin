#include <enpch.hpp>
#include "imgui.h"

#include "SceneHierarchyPanel.hpp"
#include "Engine/GameObject/Components.hpp"

namespace Engine {

    /*
    SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene3D>& context) {
        SetContext(context);
    }

    void SceneHierarchyPanel::SetContext(const Ref<Scene3D>& context) {
        m_Context = context;
    }

    void SceneHierarchyPanel::OnImGuiRender() {
        ImGui::Begin("Scene3D Hierarchy");

        auto& list = m_Context->GetRegistry().GetRegList();
        for (auto go : list) {
            GameObject GO{ go, m_Context.get() };

            DrawGameObjectNode(GO);
        }

        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
            m_SelectionContext = {};
        }

        ImGui::End();

        ImGui::Begin("Properties");
        if (m_SelectionContext) {
            DrawComponents(m_SelectionContext);
        }
        ImGui::End();
    }

    void SceneHierarchyPanel::DrawGameObjectNode(GameObject go) {
        auto& tag = go.GetComponent<TagComponent>().Name;

        ImGuiTreeNodeFlags flags = ((m_SelectionContext == go) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)go, flags, tag.c_str());
        if (ImGui::IsItemClicked()) {
            m_SelectionContext = go;
        }

        if (opened) {
            ImGui::TreePop();
        }
    }

    void SceneHierarchyPanel::DrawComponents(GameObject go) {
        if (go.HasComponent <TagComponent>()) {
            auto& tag = go.GetComponent<TagComponent>().Name;
        
            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy_s(buffer, 256, tag.c_str());
            if (ImGui::InputText("Tag", buffer, sizeof(buffer))) {
                tag = std::string(buffer);
            }
        }

        if (go.HasComponent <TransformComponent>()) {
            if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform")) {
                auto& trans = go.GetComponent<TransformComponent>().Transform;

                ImGui::DragFloat3("Position", &trans._14, 0.05f);

                ImGui::TreePop();
            }
        }
    }
    */
}
