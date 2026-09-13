

#include "tooltip_and_popup.h"

# ifdef _MSC_VER
# define portable_strcpy    strcpy_s
# define portable_sprintf   sprintf_s
# else
# define portable_strcpy    strcpy
# define portable_sprintf   sprintf
# endif


void PlotWidget(bool firstframe, int &uniqueId)
{
    // Plot Widgets =========================================================================================
    // Note: most of these plots can't be used in nodes missing, because they spawn tooltips automatically,
    // so we can't trap them in our deferred pop-up mechanism.  This causes them to fly into a random screen
    // location.
    auto plot_id = uniqueId++;
    ed::BeginNode(plot_id);
        ImGui::Text("Plot Demo");
        ed::BeginPin(uniqueId++, ed::PinKind::Input);
            ImGui::Text("-> In");
        ed::EndPin();
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(250, 0)); // Hacky magic number to space out the output pin.
        ImGui::SameLine();
        ed::BeginPin(uniqueId++, ed::PinKind::Output);
            ImGui::Text("Out ->");
        ed::EndPin();

        ImGui::PushItemWidth(300);

        // Animate a simple progress bar
        static float progress = 0.0f, progress_dir = 1.0f;
        progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
        if (progress >= +1.1f) { progress = +1.1f; progress_dir *= -1.0f; }
        if (progress <= -0.1f) { progress = -0.1f; progress_dir *= -1.0f; }


        // Typically we would use ImVec2(-1.0f,0.0f) or ImVec2(-FLT_MIN,0.0f) to use all available width,
        // or ImVec2(width,0.0f) for a specified width. ImVec2(0.0f,0.0f) uses ItemWidth.
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::Text("Progress Bar");

        float progress_saturated = (progress < 0.0f) ? 0.0f : (progress > 1.0f) ? 1.0f : progress;
        char buf[32];
        portable_sprintf(buf, "%d/%d", (int)(progress_saturated * 1753), 1753);
        ImGui::ProgressBar(progress, ImVec2(0.f, 0.f), buf);

        ImGui::PopItemWidth();
    ed::EndNode();
    if (firstframe) {
        ed::SetNodePosition(plot_id, ImVec2(850, 20));
    }
}
