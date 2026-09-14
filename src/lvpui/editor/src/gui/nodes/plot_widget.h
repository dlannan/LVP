# pragma once

# include <imgui.h>
# include <imgui_internal.h>
# include <imgui_node_editor.h>
# include <application.h>

namespace ed = ax::NodeEditor;

void PlotWidget(bool firstframe, int &uniqueId, ImVector<LinkInfo>& mLinks);