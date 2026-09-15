# pragma once

# include <imgui.h>
# include <imgui_internal.h>
# include <imgui_node_editor.h>
# include <application.h>

#include "imgui_node_links.h"

#include <string>
#include <uv.h>

class ImguiNode {
public:
    ImguiNode(Application &app, ImVector<LinkInfo> &links)
        : m_app(&app)
        , m_links(links) {}
    virtual ~ImguiNode()  = default;

    virtual void Init() {}
    virtual void PreUpdate() {}
    virtual void Update(bool firstframe, int &uniqueId) {}
    virtual void PostUpdate() {}
    virtual void Done() {}

protected:
    Application *       m_app;
    ImVector<LinkInfo> &m_links;

private:

    std::string         m_name;
};