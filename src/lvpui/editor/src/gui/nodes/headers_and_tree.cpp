
#include "headers_and_tree.h"

# ifdef _MSC_VER
# define portable_strcpy    strcpy_s
# define portable_sprintf   sprintf_s
# else
# define portable_strcpy    strcpy
# define portable_sprintf   sprintf
# endif

void HeadersAndTree(int firstframe, int &uniqueId)
{
    // Headers and Trees Demo =======================================================================================================
    // TreeNodes and Headers streatch to the entire remaining work area. To put them in nodes what we need to do is to tell
    // ImGui out work area is shorter. We can achieve that right now only by using columns API.
    //
    // Relevent bugs: https://github.com/thedmd/imgui-node-editor/issues/30
    auto header_id = uniqueId++;
    ed::BeginNode(header_id);
        ImGui::Text("Tree Widget Demo");

        // Pins Row
        ed::BeginPin(uniqueId++, ed::PinKind::Input);
            ImGui::Text("-> In");
        ed::EndPin();
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(35, 0)); //  magic number - Crude & simple way to nudge over the output pin. Consider using layout and springs
        ImGui::SameLine();
        ed::BeginPin(uniqueId++, ed::PinKind::Output);
            ImGui::Text("Out ->");
        ed::EndPin();

        // Tree column startup -------------------------------------------------------------------
        // Push dummy widget to extend node size. Columns do not do that.
        float width = 135; // bad magic numbers. used to define width of tree widget
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
        ImGui::Dummy(ImVec2(width, 0));
        ImGui::PopStyleVar();

        // Start columns, but use only first one.
        ImGui::BeginColumns("##TreeColumns", 2,
            ImGuiOldColumnFlags_NoBorder |
            ImGuiOldColumnFlags_NoResize |
            ImGuiOldColumnFlags_NoPreserveWidths |
            ImGuiOldColumnFlags_NoForceWithinWindow);

        // Adjust column width to match requested one.
        ImGui::SetColumnWidth(0, width
            + ImGui::GetStyle().WindowPadding.x
            + ImGui::GetStyle().ItemSpacing.x);
        // End of tree column startup --------------------------------------------------------------

        // Back to normal ImGui drawing, in our column.
        if (ImGui::CollapsingHeader("Open Header"))
        {
            ImGui::Text("Hello There");
            if (ImGui::TreeNode("Open Tree")) {
                static bool OP1_Bool = false;
                ImGui::Text("Checked: %s", OP1_Bool ? "true" : "false");
                ImGui::Checkbox("Option 1", &OP1_Bool);
                ImGui::TreePop();
            }
        }
        // Tree Column Shutdown
        ImGui::EndColumns();
    ed::EndNode(); // End of Tree Node Demo

    if (firstframe)
    {
        ed::SetNodePosition(header_id, ImVec2(420, 20));
    }
}