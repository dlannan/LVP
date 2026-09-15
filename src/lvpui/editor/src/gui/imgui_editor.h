# pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_node_editor.h>
#include <application.h>


#include "imgui_node.h"

#include <unordered_map>

namespace ed = ax::NodeEditor;

struct Example: public Application
{
public:
    using Application::Application;

    void OnStart() override;
    void OnStop() override;
    void OnFrame(float deltaTime) override;
    void PreFrame(float deltaTime) override;

    ed::EditorContext*   m_Context = nullptr;

    ImVector<LinkInfo>   m_Links;                // List of live links. It is dynamic unless you want to create read-only view over nodes.
    int                  m_NextLinkId = 100;     // Counter to help generate link ids. In real application this will probably based on pointer to user data structure.

    std::unordered_map<std::string, ImguiNode*>    m_nodes;
};