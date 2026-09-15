# pragma once

# include <imgui.h>
# include <imgui_internal.h>
# include <imgui_node_editor.h>
# include <application.h>

#include "imgui_node.h"

namespace ed = ax::NodeEditor;

class PlotWidget: public ImguiNode {
public:
    PlotWidget(Application& app, ImVector<LinkInfo>& links)
        : ImguiNode(app, links)
    {}
    
    void Init() override;
    void Update(bool firstframe, int &uniqueId) override;

private:
    bool inputLinked        = false;
};