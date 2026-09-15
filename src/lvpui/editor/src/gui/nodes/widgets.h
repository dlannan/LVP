# pragma once

# include <imgui.h>
# include <imgui_internal.h>
# include <imgui_node_editor.h>
# include <application.h>

#include "imgui_node.h"

namespace ed = ax::NodeEditor;

class Widgets: public ImguiNode {
public:
    Widgets(Application& app, ImVector<LinkInfo>& links)
        : ImguiNode(app, links)
    {}
    
    void Init() override;
    void Update(bool firstframe, int &uniqueId) override;

private:
    bool inputLinked        = false;
};