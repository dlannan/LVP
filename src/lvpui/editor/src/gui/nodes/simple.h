# pragma once

# include <imgui.h>
# include <imgui_internal.h>
# include <imgui_node_editor.h>
# include <application.h>

#include "imgui_node.h"

namespace ed = ax::NodeEditor;

class Simple: public ImguiNode {
public:
    Simple(Application& app, ImVector<LinkInfo>& links)
        : ImguiNode(app, links)
    {}
    
    void Init() override;
    void Update(bool firstframe, int &uniqueId) override;

private:
    ImTextureID test_tex;
    int test_tex_width = 0;
    int test_tex_height = 0;

    bool inputLinked        = false;
};