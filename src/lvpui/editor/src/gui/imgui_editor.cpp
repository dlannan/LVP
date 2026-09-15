// ===================================================================================================================
// Widget Example
// Drawing standard ImGui widgets inside the node body
//
// First, some unsorted notes about which widgets do and don't draw well inside nodes.  Run the examples to see all the allowed widgets.
//
// - Child windows with scrolling doesn't work in the node.  The child window appears in a normal node,
//   and scrolls, but its contents are floating around in the wrong location, and they are not scaled.
//   Note that you can put scrolling child windows into "deferred popups" (see next item).
// - Listboxes and combo-boxes only work in nodes with a work-around: deferring the popup calls until after the node drawing is
//   completed. Look to the popup-demo for an example.
// - Headers and trees work inside the nodes only with hacks.  This is because they attempt to span the "avaialbe width"
//   and the nodes can't tell these widgets how wide it is. The work-around is to set up a fake
//   table with a static column width, then draw your header and tree widgets in that column.
// - Clickable tabs don't work in nodes.  Tabs appear, but you cannot actually change the tab, so they're functionally useless.
// - Editable text areas work, but you have to manually manage disabling the editor shorcuts while typing is detected.
//   Look around for the call to ed::EnableShortcuts() for an example.
// - Most of the cool graph widgets can't be used because they are hard-coded in ImGui to spawn tooltips, which don't work.

#include "imgui_editor.h"

# ifdef _MSC_VER
# define portable_strcpy    strcpy_s
# define portable_sprintf   sprintf_s
# else
# define portable_strcpy    strcpy
# define portable_sprintf   sprintf
# endif


void Example::OnStart() 
{
    ed::Config config;
    config.SettingsFile = "Widgets.json";
    m_Context = ed::CreateEditor(&config);
    m_InputVideo = new InputVideo((Application &)*this, m_Links);
    m_InputVideo->Init();

    m_Widgets = new Widgets((Application &)*this, m_Links);
    m_Widgets->Init();

    m_PlotWidget = new PlotWidget((Application &)*this, m_Links);
    m_PlotWidget->Init();
}

void Example::OnStop()  
{
    ed::DestroyEditor(m_Context);
}

float elapsed = 0.0f;
float target_fps = 1.0f / 60.0f;

void Example::PreFrame(float deltaTime)  
{
    elapsed += ImGui::GetIO().DeltaTime;
    if (elapsed >= target_fps)
    {
        m_InputVideo->PreUpdate();
        // Do something after 1 second.
        elapsed -= target_fps;
    }    
}

void Example::OnFrame(float deltaTime)  
{
    static bool firstframe = true; // Used to position the nodes on startup
    auto& io = ImGui::GetIO();

    // FPS Counter Ribbon
    ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);
    ImGui::Separator();

    // Node Editor Widget
    ed::SetCurrentEditor(m_Context);
    ed::Begin("My Editor", ImVec2(0.0, 0.0f));
        int uniqueId = 1;

        m_InputVideo->Update(firstframe, uniqueId);
        m_Widgets->Update(firstframe, uniqueId);
        m_PlotWidget->Update(firstframe, uniqueId);

        // ==================================================================================================
        // Link Drawing Section

        for (auto& linkInfo : m_Links)
            ed::Link(linkInfo.Id, linkInfo.InputId, linkInfo.OutputId);

        // ==================================================================================================
        // Interaction Handling Section
        // This was coppied from BasicInteration.cpp. See that file for commented code.

        // Handle creation action ---------------------------------------------------------------------------
        if (ed::BeginCreate())
        {
            ed::PinId inputPinId, outputPinId;
            if (ed::QueryNewLink(&inputPinId, &outputPinId))
            {
                if (inputPinId && outputPinId)
                {
                    if (ed::AcceptNewItem())
                    {
                        m_Links.push_back({ ed::LinkId(m_NextLinkId++), inputPinId, outputPinId });
                        ed::Link(m_Links.back().Id, m_Links.back().InputId, m_Links.back().OutputId);
                    }
                }
            }
        }
        ed::EndCreate();

        // Handle deletion action ---------------------------------------------------------------------------
        if (ed::BeginDelete())
        {
            ed::LinkId deletedLinkId;
            while (ed::QueryDeletedLink(&deletedLinkId))
            {
                if (ed::AcceptDeletedItem())
                {
                    for (auto& link : m_Links)
                    {
                        if (link.Id == deletedLinkId)
                        {
                            m_Links.erase(&link);
                            break;
                        }
                    }
                }
            }
        }
        ed::EndDelete();


    ed::End();
    ed::SetCurrentEditor(nullptr);
    firstframe = false;
    //ImGui::ShowMetricsWindow();
    //ImGui::ShowDemoWindow();
}

