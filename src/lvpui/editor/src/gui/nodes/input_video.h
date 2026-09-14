# pragma once

# include <imgui.h>
# include <imgui_internal.h>
# include <imgui_node_editor.h>
# include <application.h>

namespace ed = ax::NodeEditor;

void InputVideoInit(Application &app);
void InputVideo(bool firstframe, int &uniqueId, ImVector<LinkInfo> &links);
void UpdateVideo();