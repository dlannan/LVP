#include "imgui_node_links.h"


bool IsPinLinked(ed::PinId id, ImVector<LinkInfo> &links)
{
    if (!id)
        return false;

    for (auto& link : links)
        if (link.InputId == id || link.OutputId == id)
            return true;

    return false;
}