

#include "simple.h"

# ifdef _MSC_VER
# define portable_strcpy    strcpy_s
# define portable_sprintf   sprintf_s
# else
# define portable_strcpy    strcpy
# define portable_sprintf   sprintf
# endif


void Simple::Init()
{
    test_tex = app.LoadTexture("data\\MyImage01.jpg");
    test_tex_width = app.GetTextureWidth(test_tex);
    test_tex_height = app.GetTextureHeight(test_tex);
}

void Simple::Update(bool firstframe, int &uniqueId)
{
    // Start drawing nodes.
    ed::BeginNode(uniqueId++);
        ImGui::Text("Node A");
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