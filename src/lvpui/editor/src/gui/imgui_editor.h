# pragma once

# include <imgui.h>
# include <imgui_internal.h>
# include <imgui_node_editor.h>
# include <application.h>

namespace ed = ax::NodeEditor;

struct Example: public Application
{
public:
    using Application::Application;

    struct LinkInfo
    {
        ed::LinkId Id;
        ed::PinId  InputId;
        ed::PinId  OutputId;
    };

    void OnStart() override;
    void OnStop() override;
    void OnFrame(float deltaTime) override;

    ed::EditorContext* m_Context = nullptr;

    ImVector<LinkInfo>   m_Links;                // List of live links. It is dynamic unless you want to create read-only view over nodes.
    int                  m_NextLinkId = 100;     // Counter to help generate link ids. In real application this will probably based on pointer to user data structure.
};