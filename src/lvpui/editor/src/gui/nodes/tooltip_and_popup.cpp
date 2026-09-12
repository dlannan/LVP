
#include "tooltip_and_popup.h"

# ifdef _MSC_VER
# define portable_strcpy    strcpy_s
# define portable_sprintf   sprintf_s
# else
# define portable_strcpy    strcpy
# define portable_sprintf   sprintf
# endif


void ToolTipAndPopup(bool firstframe, int &uniqueId)
{
    // Tool Tip & Pop-up Demo =====================================================================================
    // Tooltips, combo-boxes, drop-down menus need to use a work-around to place the "overlay window" in the canvas.
    // To do this, we must defer the popup calls until after we're done drawing the node material.
    //
    // Relevent bugs:  https://github.com/thedmd/imgui-node-editor/issues/48
    auto popup_id = uniqueId++;
    ed::BeginNode(popup_id);
        ImGui::Text("Tool Tip & Pop-up Demo");
        ed::BeginPin(uniqueId++, ed::PinKind::Input);
            ImGui::Text("-> In");
        ed::EndPin();
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(85, 0)); // Hacky magic number to space out the output pin.
        ImGui::SameLine();
        ed::BeginPin(uniqueId++, ed::PinKind::Output);
            ImGui::Text("Out ->");
        ed::EndPin();

        // Tooltip example
        ImGui::Text("Hover over me");
        static bool do_tooltip = false;
        do_tooltip = ImGui::IsItemHovered() ? true : false;
        ImGui::SameLine();
        ImGui::Text("- or me");
        static bool do_adv_tooltip = false;
        do_adv_tooltip = ImGui::IsItemHovered() ? true : false;

        // Use AlignTextToFramePadding() to align text baseline to the baseline of framed elements
        // (otherwise a Text+SameLine+Button sequence will have the text a little too high by default)
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Option:");
        ImGui::SameLine();
        static char popup_text[128] = "Pick one!";
        static bool do_popup = false;
        if (ImGui::Button(popup_text)) {
            do_popup = true;	// Instead of saying OpenPopup() here, we set this bool, which is used later in the Deferred Pop-up Section
        }
    ed::EndNode();
    if (firstframe) {
        ed::SetNodePosition(popup_id, ImVec2(610, 20));
    }

    // --------------------------------------------------------------------------------------------------
    // Deferred Pop-up Section

    // This entire section needs to be bounded by Suspend/Resume!  These calls pop us out of "node canvas coordinates"
    // and draw the popups in a reasonable screen location.
    ed::Suspend();
    // There is some stately stuff happening here.  You call "open popup" exactly once, and this
    // causes it to stick open for many frames until the user makes a selection in the popup, or clicks off to dismiss.
    // More importantly, this is done inside Suspend(), so it loads the popup with the correct screen coordinates!
    if (do_popup) {
        ImGui::OpenPopup("popup_button"); // Cause openpopup to stick open.
        do_popup = false; // disable bool so that if we click off the popup, it doesn't open the next frame.
    }

    // This is the actual popup Gui drawing section.
    if (ImGui::BeginPopup("popup_button")) {
        // Note: if it weren't for the child window, we would have to PushItemWidth() here to avoid a crash!
        ImGui::TextDisabled("Pick One:");
        ImGui::BeginChild("popup_scroller", ImVec2(100, 100), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        if (ImGui::Button("Option 1")) {
            portable_strcpy(popup_text, "Option 1");
            ImGui::CloseCurrentPopup();  // These calls revoke the popup open state, which was set by OpenPopup above.
        }
        if (ImGui::Button("Option 2")) {
            portable_strcpy(popup_text, "Option 2");
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::Button("Option 3")) {
            portable_strcpy(popup_text, "Option 3");
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::Button("Option 4")) {
            portable_strcpy(popup_text, "Option 4");
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndChild();
        ImGui::EndPopup(); // Note this does not do anything to the popup open/close state. It just terminates the content declaration.
    }

    // Handle the simple tooltip
    if (do_tooltip)
        ImGui::SetTooltip("I am a tooltip");

    // Handle the advanced tooltip
    if (do_adv_tooltip) {
        ImGui::BeginTooltip();
        ImGui::Text("I am a fancy tooltip");
        static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
        ImGui::PlotLines("Curve", arr, IM_ARRAYSIZE(arr));
        ImGui::EndTooltip();
    }

    ed::Resume();
    // End of "Deferred Pop-up section"
}