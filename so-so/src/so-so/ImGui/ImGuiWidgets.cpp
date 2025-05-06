#include "sspch.h"
#include "ImGuiWidgets.h"

#include "imgui_internal.h"


namespace soso::UI {

    bool ToggleSwitch(const char* label, bool* v) {

        ImVec2 p = ImGui::GetCursorScreenPos();
        ImDrawList* dl = ImGui::GetWindowDrawList();

        float h = ImGui::GetFrameHeight();
        float w = h * 1.55f;
        float r = h * 0.50f;

        // Use label as str_id
        ImGui::InvisibleButton(label, ImVec2(w, h));
        bool clicked = ImGui::IsItemClicked();
        if (clicked)
            *v = !*v;

        float t = *v ? 1.0f : 0.0f;
        ImGuiContext& g = *ImGui::GetCurrentContext();
        const float SPEED = 0.08f;
        if (g.LastActiveId == g.CurrentWindow->GetID(label)) {
            float a = ImSaturate(g.LastActiveIdTimer / SPEED);
            t = *v ? a : (1.0f - a);
        }

        ImU32 colBg;
        if (ImGui::IsItemHovered())
            colBg = ImGui::GetColorU32(ImLerp(ImVec4(0.78f, 0.78f, 0.78f, 1.0f), ImVec4(0.64f, 0.83f, 0.34f, 1.0f), t));
        else
            colBg = ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.85f, 0.85f, 1.0f), ImVec4(0.56f, 0.83f, 0.26f, 1.0f), t));


        dl->AddRectFilled(p, ImVec2(p.x + w, p.y + h), colBg, h * 0.5f);
        dl->AddCircleFilled(ImVec2(p.x + r + t * (w - r * 2.0f), p.y + r), r - 1.5f, IM_COL32(255, 255, 255, 255));

        ImGui::SameLine();
        ImGui::TextUnformatted(label);

        return clicked;
    }

}