// ==============================================================================
// src/editor/gui/output_log.cpp
// ==============================================================================
#include "output_log.hpp"
#include <imgui.h>

namespace arx {

void OutputLog::render() {
    if (!ImGui::Begin("Output")) { ImGui::End(); return; }

    if (ImGui::Button("Clear")) {
        std::lock_guard<std::mutex> lock(mtx_);
        lines_.clear();
    }
    ImGui::SameLine();
    ImGui::Checkbox("Auto-scroll", &auto_scroll_);

    ImGui::SameLine();
    if (ImGui::Button("Copy")) {
        std::string all;
        std::lock_guard<std::mutex> lock(mtx_);
        for (const auto& l : lines_) all += l.text + "\n";
        ImGui::SetClipboardText(all.c_str());
    }

    ImGui::Separator();

    ImGui::BeginChild("scroll", ImVec2(0,0), false,
                       ImGuiWindowFlags_HorizontalScrollbar);

    std::lock_guard<std::mutex> lock(mtx_);
    for (const auto& l : lines_) {
        ImGui::PushStyleColor(ImGuiCol_Text, l.color);
        ImGui::TextUnformatted(l.text.c_str());
        ImGui::PopStyleColor();
    }

    if (auto_scroll_ && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();
    ImGui::End();
}

} // namespace arx
