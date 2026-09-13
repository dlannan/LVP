

#include "simple.h"

# ifdef _MSC_VER
# define portable_strcpy    strcpy_s
# define portable_sprintf   sprintf_s
# else
# define portable_strcpy    strcpy
# define portable_sprintf   sprintf
# endif

static ImTextureID test_tex;
static int test_tex_width = 0;
static int test_tex_height = 0;

void SimpleNodeInit(Application &app)
{
    test_tex = app.LoadTexture("data\\MyImage01.jpg");
    test_tex_width = app.GetTextureWidth(test_tex);
    test_tex_height = app.GetTextureHeight(test_tex);
}

void SimpleNode(bool firstframe, int &uniqueId)
{
    // Start drawing nodes.
    ed::BeginNode(uniqueId++);
        ImGui::Text("Node A");
        ed::BeginPin(uniqueId++, ed::PinKind::Input);
            ImGui::Text("-> In");
        ed::EndPin();
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(test_tex_width - 90, 0)); // Hacky magic number to space out the output pin.
        ImGui::SameLine();
        ed::BeginPin(uniqueId++, ed::PinKind::Output);
            ImGui::Text("Out ->");
        ed::EndPin();

        ImGui::Text("pointer = %p", test_tex);
        ImGui::Text("size = %d x %d", test_tex_width, test_tex_height);
        ImGui::Image((ImTextureID)(intptr_t)test_tex, ImVec2(test_tex_width, test_tex_height));
    ed::EndNode();
}