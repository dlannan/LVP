# pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_node_editor.h>
#include <application.h>

#include "imgui_node.h"

extern "C" {
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb_image.h"
}

namespace ed = ax::NodeEditor;

class InputVideo: public ImguiNode {
public:
    InputVideo(Application& app, ImVector<LinkInfo>& links)
        : ImguiNode(app, links)
    {}
    
    void Init() override;
    void PreUpdate() override;
    void Update(bool firstframe, int &uniqueId) override;
    void Done() override;

private:
    ImTextureID test_tex;
    int test_tex_width      = 0;
    int test_tex_height     = 0;
    stbi_uc *tex_data       = nullptr;
    stbi_uc *data_buffs[2]  = { nullptr, nullptr };
    int currbuff            = 0;    

    bool inputLinked        = false;
};