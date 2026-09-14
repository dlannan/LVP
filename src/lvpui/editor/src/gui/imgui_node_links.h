# pragma once

# include <imgui.h>
# include <imgui_internal.h>
# include <imgui_node_editor.h>
# include <application.h>

namespace ed = ax::NodeEditor;

struct LinkInfo
{
    ed::LinkId Id;
    ed::PinId  InputId;
    ed::PinId  OutputId;
};


// TODO Build a Node/Link Management here.
bool IsPinLinked(ed::PinId id, ImVector<LinkInfo> &links);