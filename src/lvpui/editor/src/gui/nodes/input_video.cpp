

#include "imgui_node_links.h"

#include "input_video.h"

# ifdef _MSC_VER
# define portable_strcpy    strcpy_s
# define portable_sprintf   sprintf_s
# else
# define portable_strcpy    strcpy
# define portable_sprintf   sprintf
# endif

#include <string>
#include <iostream>

void Rotate90CW(
    const unsigned char* src,
    unsigned char* dst,
    int width,
    int height)
{
    constexpr int BPP = 4;

    const int newWidth  = height;
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            // (x, y) -> (height - 1 - y, x)
            int dstX = height - 1 - y;
            int dstY = x;

            const unsigned char* srcPixel =
                src + (y * width + x) * BPP;

            unsigned char* dstPixel =
                dst + (dstY * newWidth + dstX) * BPP;

            memcpy(dstPixel, srcPixel, BPP);
        }
    }
}

void InputVideo::Init()
{
    int width = 0, height = 0, component = 0;
    tex_data = stbi_load("data\\MyImage01.jpg", &width, &height, &component, 4);
    if (tex_data) {
        test_tex = m_app->CreateDynamicTexture(tex_data, width, height);
        test_tex_width = width;
        test_tex_height = height;
        data_buffs[0] = tex_data;
        data_buffs[1] = new stbi_uc[width * height * 4];
        std::cout << "Loaded Texture...\n";
    }
}

void InputVideo::PreUpdate()
{
    if(!inputLinked) return;
    Rotate90CW(data_buffs[currbuff], data_buffs[1-currbuff], test_tex_width, test_tex_height);
    currbuff = 1-currbuff;
    bool ok = m_app->UpdateTexture(test_tex, data_buffs[currbuff]);
    //std::cout << "Texture Update:" << ok << "\n";
}

void InputVideo::Update(bool firstframe, int &uniqueId) 
{
    // Start drawing nodes.
    ed::BeginNode(uniqueId++);
        ImGui::Text("Input Video");
        ed::BeginPin(uniqueId++, ed::PinKind::Input);
            ImGui::Text("-> In");
            inputLinked = IsPinLinked(uniqueId-1, m_links);
        ed::EndPin();
        ImGui::SameLine();
        ImGui::Dummy(ImVec2((float)(test_tex_width - 90), 0)); // Hacky magic number to space out the output pin.
        ImGui::SameLine();
        ed::BeginPin(uniqueId++, ed::PinKind::Output);
            ImGui::Text("Out ->");
        ed::EndPin();

        ImGui::Text("pointer = %p", test_tex);
        ImGui::Text("size = %d x %d", test_tex_width, test_tex_height);
        ImGui::Image((ImTextureID)(intptr_t)test_tex, ImVec2((float)test_tex_width, (float)test_tex_height));
    ed::EndNode();
}

void InputVideo::Done() 
{
    
}

