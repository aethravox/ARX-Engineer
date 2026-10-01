// ==============================================================================
// src/editor/gui/theme_editor.cpp — Editor de tema visual en vivo.
// ==============================================================================
#include "theme_editor.hpp"
#include "core/logging.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <fstream>
#include <sstream>
#include <cstring>
#include <regex>

namespace arx {

// Helper: convertir ImVec4 a string hex (#RRGGBBAA)
static std::string col_to_hex(const ImVec4& c) {
    auto clamp = [](float v) { return v < 0 ? 0 : (v > 1 ? 255 : (int)(v * 255 + 0.5f)); };
    char buf[16];
    std::snprintf(buf, sizeof(buf), "#%02X%02X%02X%02X",
                  clamp(c.x), clamp(c.y), clamp(c.z), clamp(c.w));
    return buf;
}

// Helper: parsear hex a ImVec4
static ImVec4 hex_to_col(const std::string& hex) {
    if (hex.size() < 7 || hex[0] != '#') return ImVec4(0, 0, 0, 1);
    unsigned int r = 0, g = 0, b = 0, a = 255;
    if (hex.size() >= 7) std::sscanf(hex.c_str() + 1, "%02X%02X%02X", &r, &g, &b);
    if (hex.size() >= 9) std::sscanf(hex.c_str() + 7, "%02X", &a);
    return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

// ==============================================================================
// Presets de temas. Cada preset es una lista de (col_index, color).
// ==============================================================================
struct PresetColor { ImGuiCol idx; ImVec4 color; };

static const PresetColor kPresetARXDark[] = {
    {ImGuiCol_WindowBg,         ImVec4(0.10f, 0.10f, 0.13f, 1.0f)},
    {ImGuiCol_ChildBg,          ImVec4(0.13f, 0.13f, 0.17f, 1.0f)},
    {ImGuiCol_PopupBg,          ImVec4(0.13f, 0.13f, 0.17f, 0.98f)},
    {ImGuiCol_Border,           ImVec4(0.22f, 0.22f, 0.28f, 1.0f)},
    {ImGuiCol_Text,             ImVec4(0.91f, 0.91f, 0.94f, 1.0f)},
    {ImGuiCol_TextDisabled,     ImVec4(0.50f, 0.50f, 0.55f, 1.0f)},
    {ImGuiCol_FrameBg,          ImVec4(0.18f, 0.18f, 0.22f, 1.0f)},
    {ImGuiCol_FrameBgHovered,   ImVec4(0.26f, 0.26f, 0.32f, 1.0f)},
    {ImGuiCol_FrameBgActive,    ImVec4(0.34f, 0.34f, 0.42f, 1.0f)},
    {ImGuiCol_TitleBg,          ImVec4(0.10f, 0.10f, 0.13f, 1.0f)},
    {ImGuiCol_TitleBgActive,    ImVec4(0.18f, 0.18f, 0.22f, 1.0f)},
    {ImGuiCol_MenuBarBg,        ImVec4(0.13f, 0.13f, 0.17f, 1.0f)},
    {ImGuiCol_ScrollbarBg,      ImVec4(0.10f, 0.10f, 0.13f, 1.0f)},
    {ImGuiCol_ScrollbarGrab,    ImVec4(0.30f, 0.30f, 0.34f, 1.0f)},
    {ImGuiCol_CheckMark,        ImVec4(0.49f, 0.23f, 0.93f, 1.0f)},
    {ImGuiCol_SliderGrab,       ImVec4(0.49f, 0.23f, 0.93f, 1.0f)},
    {ImGuiCol_Button,           ImVec4(0.22f, 0.22f, 0.28f, 1.0f)},
    {ImGuiCol_ButtonHovered,    ImVec4(0.49f, 0.23f, 0.93f, 1.0f)},
    {ImGuiCol_ButtonActive,     ImVec4(0.60f, 0.30f, 1.00f, 1.0f)},
    {ImGuiCol_Header,           ImVec4(0.49f, 0.23f, 0.93f, 0.5f)},
    {ImGuiCol_HeaderHovered,    ImVec4(0.49f, 0.23f, 0.93f, 0.8f)},
    {ImGuiCol_HeaderActive,     ImVec4(0.49f, 0.23f, 0.93f, 1.0f)},
    {ImGuiCol_Tab,              ImVec4(0.15f, 0.15f, 0.18f, 1.0f)},
    {ImGuiCol_TabHovered,       ImVec4(0.49f, 0.23f, 0.93f, 0.7f)},
    {ImGuiCol_TabActive,        ImVec4(0.49f, 0.23f, 0.93f, 1.0f)},
    {ImGuiCol_DockingPreview,   ImVec4(0.10f, 0.85f, 0.92f, 0.5f)},
    {ImGuiCol_DockingEmptyBg,   ImVec4(0.05f, 0.05f, 0.07f, 1.0f)},
};

static const PresetColor kPresetGodot[] = {
    {ImGuiCol_WindowBg,         ImVec4(0.12f, 0.14f, 0.16f, 1.0f)},
    {ImGuiCol_ChildBg,          ImVec4(0.15f, 0.17f, 0.19f, 1.0f)},
    {ImGuiCol_PopupBg,          ImVec4(0.15f, 0.17f, 0.19f, 0.98f)},
    {ImGuiCol_Border,           ImVec4(0.25f, 0.27f, 0.29f, 1.0f)},
    {ImGuiCol_Text,             ImVec4(0.90f, 0.90f, 0.90f, 1.0f)},
    {ImGuiCol_TextDisabled,     ImVec4(0.50f, 0.50f, 0.50f, 1.0f)},
    {ImGuiCol_FrameBg,          ImVec4(0.20f, 0.22f, 0.24f, 1.0f)},
    {ImGuiCol_FrameBgHovered,   ImVec4(0.28f, 0.30f, 0.32f, 1.0f)},
    {ImGuiCol_FrameBgActive,    ImVec4(0.36f, 0.38f, 0.40f, 1.0f)},
    {ImGuiCol_TitleBg,          ImVec4(0.12f, 0.14f, 0.16f, 1.0f)},
    {ImGuiCol_TitleBgActive,    ImVec4(0.20f, 0.22f, 0.24f, 1.0f)},
    {ImGuiCol_MenuBarBg,        ImVec4(0.15f, 0.17f, 0.19f, 1.0f)},
    {ImGuiCol_ScrollbarBg,      ImVec4(0.12f, 0.14f, 0.16f, 1.0f)},
    {ImGuiCol_ScrollbarGrab,    ImVec4(0.32f, 0.34f, 0.36f, 1.0f)},
    {ImGuiCol_CheckMark,        ImVec4(0.40f, 0.76f, 0.21f, 1.0f)},  // Godot green
    {ImGuiCol_SliderGrab,       ImVec4(0.40f, 0.76f, 0.21f, 1.0f)},
    {ImGuiCol_Button,           ImVec4(0.24f, 0.26f, 0.28f, 1.0f)},
    {ImGuiCol_ButtonHovered,    ImVec4(0.40f, 0.76f, 0.21f, 1.0f)},
    {ImGuiCol_ButtonActive,     ImVec4(0.55f, 0.86f, 0.31f, 1.0f)},
    {ImGuiCol_Header,           ImVec4(0.40f, 0.76f, 0.21f, 0.5f)},
    {ImGuiCol_HeaderHovered,    ImVec4(0.40f, 0.76f, 0.21f, 0.8f)},
    {ImGuiCol_HeaderActive,     ImVec4(0.40f, 0.76f, 0.21f, 1.0f)},
    {ImGuiCol_Tab,              ImVec4(0.17f, 0.19f, 0.21f, 1.0f)},
    {ImGuiCol_TabHovered,       ImVec4(0.40f, 0.76f, 0.21f, 0.7f)},
    {ImGuiCol_TabActive,        ImVec4(0.40f, 0.76f, 0.21f, 1.0f)},
    {ImGuiCol_DockingPreview,   ImVec4(0.40f, 0.76f, 0.21f, 0.5f)},
    {ImGuiCol_DockingEmptyBg,   ImVec4(0.08f, 0.10f, 0.12f, 1.0f)},
};

static const PresetColor kPresetLight[] = {
    {ImGuiCol_WindowBg,         ImVec4(0.94f, 0.94f, 0.94f, 1.0f)},
    {ImGuiCol_ChildBg,          ImVec4(0.96f, 0.96f, 0.96f, 1.0f)},
    {ImGuiCol_PopupBg,          ImVec4(1.00f, 1.00f, 1.00f, 0.98f)},
    {ImGuiCol_Border,           ImVec4(0.72f, 0.72f, 0.76f, 1.0f)},
    {ImGuiCol_Text,             ImVec4(0.10f, 0.10f, 0.10f, 1.0f)},
    {ImGuiCol_TextDisabled,     ImVec4(0.50f, 0.50f, 0.50f, 1.0f)},
    {ImGuiCol_FrameBg,          ImVec4(0.86f, 0.86f, 0.86f, 1.0f)},
    {ImGuiCol_FrameBgHovered,   ImVec4(0.78f, 0.78f, 0.82f, 1.0f)},
    {ImGuiCol_FrameBgActive,    ImVec4(0.70f, 0.70f, 0.74f, 1.0f)},
    {ImGuiCol_TitleBg,          ImVec4(0.84f, 0.84f, 0.84f, 1.0f)},
    {ImGuiCol_TitleBgActive,    ImVec4(0.78f, 0.78f, 0.78f, 1.0f)},
    {ImGuiCol_MenuBarBg,        ImVec4(0.90f, 0.90f, 0.90f, 1.0f)},
    {ImGuiCol_ScrollbarBg,      ImVec4(0.94f, 0.94f, 0.94f, 1.0f)},
    {ImGuiCol_ScrollbarGrab,    ImVec4(0.65f, 0.65f, 0.65f, 1.0f)},
    {ImGuiCol_CheckMark,        ImVec4(0.20f, 0.45f, 0.85f, 1.0f)},
    {ImGuiCol_SliderGrab,       ImVec4(0.20f, 0.45f, 0.85f, 1.0f)},
    {ImGuiCol_Button,           ImVec4(0.78f, 0.78f, 0.82f, 1.0f)},
    {ImGuiCol_ButtonHovered,    ImVec4(0.20f, 0.45f, 0.85f, 1.0f)},
    {ImGuiCol_ButtonActive,     ImVec4(0.30f, 0.55f, 0.95f, 1.0f)},
    {ImGuiCol_Header,           ImVec4(0.20f, 0.45f, 0.85f, 0.5f)},
    {ImGuiCol_HeaderHovered,    ImVec4(0.20f, 0.45f, 0.85f, 0.8f)},
    {ImGuiCol_HeaderActive,     ImVec4(0.20f, 0.45f, 0.85f, 1.0f)},
    {ImGuiCol_Tab,              ImVec4(0.84f, 0.84f, 0.84f, 1.0f)},
    {ImGuiCol_TabHovered,       ImVec4(0.20f, 0.45f, 0.85f, 0.7f)},
    {ImGuiCol_TabActive,        ImVec4(0.20f, 0.45f, 0.85f, 1.0f)},
    {ImGuiCol_DockingPreview,   ImVec4(0.20f, 0.45f, 0.85f, 0.5f)},
    {ImGuiCol_DockingEmptyBg,   ImVec4(0.88f, 0.88f, 0.88f, 1.0f)},
};

static const PresetColor kPresetSolarized[] = {
    {ImGuiCol_WindowBg,         ImVec4(0.03f, 0.06f, 0.10f, 1.0f)},  // base03
    {ImGuiCol_ChildBg,          ImVec4(0.05f, 0.08f, 0.12f, 1.0f)},  // base02
    {ImGuiCol_PopupBg,          ImVec4(0.05f, 0.08f, 0.12f, 0.98f)},
    {ImGuiCol_Border,           ImVec4(0.15f, 0.20f, 0.25f, 1.0f)},
    {ImGuiCol_Text,             ImVec4(0.90f, 0.85f, 0.70f, 1.0f)},  // base0
    {ImGuiCol_TextDisabled,     ImVec4(0.50f, 0.50f, 0.50f, 1.0f)},
    {ImGuiCol_FrameBg,          ImVec4(0.10f, 0.13f, 0.17f, 1.0f)},
    {ImGuiCol_FrameBgHovered,   ImVec4(0.18f, 0.21f, 0.25f, 1.0f)},
    {ImGuiCol_FrameBgActive,    ImVec4(0.26f, 0.29f, 0.33f, 1.0f)},
    {ImGuiCol_TitleBg,          ImVec4(0.03f, 0.06f, 0.10f, 1.0f)},
    {ImGuiCol_TitleBgActive,    ImVec4(0.10f, 0.13f, 0.17f, 1.0f)},
    {ImGuiCol_MenuBarBg,        ImVec4(0.05f, 0.08f, 0.12f, 1.0f)},
    {ImGuiCol_ScrollbarBg,      ImVec4(0.03f, 0.06f, 0.10f, 1.0f)},
    {ImGuiCol_ScrollbarGrab,    ImVec4(0.25f, 0.28f, 0.32f, 1.0f)},
    {ImGuiCol_CheckMark,        ImVec4(0.71f, 0.54f, 0.00f, 1.0f)},  // yellow
    {ImGuiCol_SliderGrab,       ImVec4(0.71f, 0.54f, 0.00f, 1.0f)},
    {ImGuiCol_Button,           ImVec4(0.15f, 0.20f, 0.25f, 1.0f)},
    {ImGuiCol_ButtonHovered,    ImVec4(0.71f, 0.54f, 0.00f, 1.0f)},
    {ImGuiCol_ButtonActive,     ImVec4(0.85f, 0.65f, 0.10f, 1.0f)},
    {ImGuiCol_Header,           ImVec4(0.71f, 0.54f, 0.00f, 0.5f)},
    {ImGuiCol_HeaderHovered,    ImVec4(0.71f, 0.54f, 0.00f, 0.8f)},
    {ImGuiCol_HeaderActive,     ImVec4(0.71f, 0.54f, 0.00f, 1.0f)},
    {ImGuiCol_Tab,              ImVec4(0.10f, 0.13f, 0.17f, 1.0f)},
    {ImGuiCol_TabHovered,       ImVec4(0.71f, 0.54f, 0.00f, 0.7f)},
    {ImGuiCol_TabActive,        ImVec4(0.71f, 0.54f, 0.00f, 1.0f)},
    {ImGuiCol_DockingPreview,   ImVec4(0.71f, 0.54f, 0.00f, 0.5f)},
    {ImGuiCol_DockingEmptyBg,   ImVec4(0.02f, 0.05f, 0.08f, 1.0f)},
};

static const PresetColor kPresetVSCode[] = {
    {ImGuiCol_WindowBg,         ImVec4(0.10f, 0.10f, 0.10f, 1.0f)},  // #1e1e1e aprox
    {ImGuiCol_ChildBg,          ImVec4(0.13f, 0.13f, 0.13f, 1.0f)},
    {ImGuiCol_PopupBg,          ImVec4(0.13f, 0.13f, 0.13f, 0.98f)},
    {ImGuiCol_Border,           ImVec4(0.20f, 0.20f, 0.20f, 1.0f)},
    {ImGuiCol_Text,             ImVec4(0.85f, 0.85f, 0.85f, 1.0f)},
    {ImGuiCol_TextDisabled,     ImVec4(0.50f, 0.50f, 0.50f, 1.0f)},
    {ImGuiCol_FrameBg,          ImVec4(0.18f, 0.18f, 0.18f, 1.0f)},
    {ImGuiCol_FrameBgHovered,   ImVec4(0.25f, 0.25f, 0.25f, 1.0f)},
    {ImGuiCol_FrameBgActive,    ImVec4(0.32f, 0.32f, 0.32f, 1.0f)},
    {ImGuiCol_TitleBg,          ImVec4(0.10f, 0.10f, 0.10f, 1.0f)},
    {ImGuiCol_TitleBgActive,    ImVec4(0.15f, 0.15f, 0.15f, 1.0f)},
    {ImGuiCol_MenuBarBg,        ImVec4(0.13f, 0.13f, 0.13f, 1.0f)},
    {ImGuiCol_ScrollbarBg,      ImVec4(0.10f, 0.10f, 0.10f, 1.0f)},
    {ImGuiCol_ScrollbarGrab,    ImVec4(0.30f, 0.30f, 0.30f, 1.0f)},
    {ImGuiCol_CheckMark,        ImVec4(0.00f, 0.47f, 0.84f, 1.0f)},  // blue accent
    {ImGuiCol_SliderGrab,       ImVec4(0.00f, 0.47f, 0.84f, 1.0f)},
    {ImGuiCol_Button,           ImVec4(0.20f, 0.20f, 0.20f, 1.0f)},
    {ImGuiCol_ButtonHovered,    ImVec4(0.00f, 0.47f, 0.84f, 1.0f)},
    {ImGuiCol_ButtonActive,     ImVec4(0.13f, 0.58f, 0.95f, 1.0f)},
    {ImGuiCol_Header,           ImVec4(0.00f, 0.47f, 0.84f, 0.5f)},
    {ImGuiCol_HeaderHovered,    ImVec4(0.00f, 0.47f, 0.84f, 0.8f)},
    {ImGuiCol_HeaderActive,     ImVec4(0.00f, 0.47f, 0.84f, 1.0f)},
    {ImGuiCol_Tab,              ImVec4(0.13f, 0.13f, 0.13f, 1.0f)},
    {ImGuiCol_TabHovered,       ImVec4(0.00f, 0.47f, 0.84f, 0.7f)},
    {ImGuiCol_TabActive,        ImVec4(0.00f, 0.47f, 0.84f, 1.0f)},
    {ImGuiCol_DockingPreview,   ImVec4(0.00f, 0.47f, 0.84f, 0.5f)},
    {ImGuiCol_DockingEmptyBg,   ImVec4(0.07f, 0.07f, 0.07f, 1.0f)},
};

// ==============================================================================
void ThemeEditor::apply_preset(const std::string& name) {
    ImGuiStyle& s = ImGui::GetStyle();
    ImGui::StyleColorsDark(&s);  // empezar de un base oscuro

    const PresetColor* preset = nullptr;
    int count = 0;
    if (name == "ARX Dark") {
        preset = kPresetARXDark; count = sizeof(kPresetARXDark) / sizeof(PresetColor);
    } else if (name == "Godot") {
        preset = kPresetGodot; count = sizeof(kPresetGodot) / sizeof(PresetColor);
    } else if (name == "Light") {
        preset = kPresetLight; count = sizeof(kPresetLight) / sizeof(PresetColor);
    } else if (name == "Solarized") {
        preset = kPresetSolarized; count = sizeof(kPresetSolarized) / sizeof(PresetColor);
    } else if (name == "VS Code") {
        preset = kPresetVSCode; count = sizeof(kPresetVSCode) / sizeof(PresetColor);
    }

    if (preset) {
        for (int i = 0; i < count; ++i) {
            s.Colors[preset[i].idx] = preset[i].color;
        }
        current_preset_ = name;
        ARX_LOG_INFO("ThemeEditor: preset '{}' aplicado", name);
    }
}

// ==============================================================================
void ThemeEditor::render() {
    if (!visible_) return;

    // Forzar tamaño/posición la primera vez y traer al frente
    static bool first_render = true;
    if (first_render) {
        first_render = false;
        ImGui::SetNextWindowSize(ImVec2(500, 600), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(340, 150), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowFocus();
    }

    // Usar ImGuiWindowFlags_NoDocking para que sea floating window
    // (y no se oculte detrás del editor fullscreen)
    if (!ImGui::Begin("Theme Editor", &visible_,
                       ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse)) {
        ImGui::End();
        return;
    }

    render_presets_();
    ImGui::Separator();

    if (ImGui::BeginTabBar("ThemeTabs")) {
        if (ImGui::BeginTabItem("Colors")) {
            render_color_editor_();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Style")) {
            render_style_editor_();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::Separator();
    // Save/Load
    if (ImGui::Button("Save to arx_theme.json")) {
        save_to_file("arx_theme.json");
    }
    ImGui::SameLine();
    if (ImGui::Button("Load from arx_theme.json")) {
        load_from_file("arx_theme.json");
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset to Preset")) {
        apply_preset(current_preset_);
    }

    ImGui::End();
}

// ==============================================================================
void ThemeEditor::render_presets_() {
    ImGui::TextDisabled("Preset: %s", current_preset_.c_str());
    ImGui::SameLine();
    if (ImGui::Button("ARX Dark"))  apply_preset("ARX Dark");
    ImGui::SameLine();
    if (ImGui::Button("Godot"))    apply_preset("Godot");
    ImGui::SameLine();
    if (ImGui::Button("VS Code"))  apply_preset("VS Code");
    ImGui::SameLine();
    if (ImGui::Button("Light"))    apply_preset("Light");
    ImGui::SameLine();
    if (ImGui::Button("Solarized")) apply_preset("Solarized");
}

// ==============================================================================
void ThemeEditor::render_color_editor_() {
    ImGuiStyle& s = ImGui::GetStyle();
    for (int i = 0; i < ImGuiCol_COUNT; ++i) {
        const char* name = ImGui::GetStyleColorName(i);
        ImVec4 col = s.Colors[i];
        ImGui::PushID(i);
        if (ImGui::ColorEdit4(name, &col.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
            s.Colors[i] = col;
        }
        ImGui::SameLine();
        ImGui::TextDisabled("%s", col_to_hex(col).c_str());
        ImGui::PopID();
    }
}

// ==============================================================================
void ThemeEditor::render_style_editor_() {
    ImGuiStyle& s = ImGui::GetStyle();
    ImGui::SliderFloat("WindowRounding",    &s.WindowRounding,    0.0f, 16.0f);
    ImGui::SliderFloat("ChildRounding",     &s.ChildRounding,     0.0f, 16.0f);
    ImGui::SliderFloat("FrameRounding",     &s.FrameRounding,     0.0f, 16.0f);
    ImGui::SliderFloat("GrabRounding",      &s.GrabRounding,      0.0f, 16.0f);
    ImGui::SliderFloat("TabRounding",       &s.TabRounding,       0.0f, 16.0f);
    ImGui::SliderFloat("ScrollbarRounding", &s.ScrollbarRounding, 0.0f, 16.0f);
    ImGui::Separator();
    ImGui::SliderFloat2("WindowPadding",  (float*)&s.WindowPadding,  0.0f, 20.0f);
    ImGui::SliderFloat2("FramePadding",   (float*)&s.FramePadding,   0.0f, 20.0f);
    ImGui::SliderFloat2("ItemSpacing",    (float*)&s.ItemSpacing,    0.0f, 20.0f);
    ImGui::SliderFloat2("ItemInnerSpacing",(float*)&s.ItemInnerSpacing, 0.0f, 20.0f);
    ImGui::Separator();
    ImGui::SliderFloat("WindowBorderSize", &s.WindowBorderSize, 0.0f, 5.0f);
    ImGui::SliderFloat("FrameBorderSize",  &s.FrameBorderSize,  0.0f, 5.0f);
    ImGui::SliderFloat("ScrollbarSize",    &s.ScrollbarSize,    5.0f, 30.0f);
}

// ==============================================================================
void ThemeEditor::save_to_file(const std::string& path) {
    ImGuiStyle& s = ImGui::GetStyle();
    std::ofstream f(path);
    if (!f.is_open()) {
        ARX_LOG_ERROR("ThemeEditor: no se pudo escribir '{}'", path);
        return;
    }
    f << "{\n";
    f << "  \"preset\": \"" << current_preset_ << "\",\n";
    f << "  \"colors\": {\n";
    for (int i = 0; i < ImGuiCol_COUNT; ++i) {
        const ImVec4& c = s.Colors[i];
        f << "    \"" << ImGui::GetStyleColorName(i) << "\": "
          << "\"" << col_to_hex(c) << "\"";
        if (i < ImGuiCol_COUNT - 1) f << ",";
        f << "\n";
    }
    f << "  },\n";
    f << "  \"style\": {\n";
    f << "    \"WindowRounding\": " << s.WindowRounding << ",\n";
    f << "    \"ChildRounding\": " << s.ChildRounding << ",\n";
    f << "    \"FrameRounding\": " << s.FrameRounding << ",\n";
    f << "    \"GrabRounding\": " << s.GrabRounding << ",\n";
    f << "    \"TabRounding\": " << s.TabRounding << ",\n";
    f << "    \"ScrollbarRounding\": " << s.ScrollbarRounding << ",\n";
    f << "    \"WindowBorderSize\": " << s.WindowBorderSize << ",\n";
    f << "    \"FrameBorderSize\": " << s.FrameBorderSize << ",\n";
    f << "    \"ScrollbarSize\": " << s.ScrollbarSize << "\n";
    f << "  }\n";
    f << "}\n";
    f.close();
    ARX_LOG_INFO("ThemeEditor: tema guardado en '{}'", path);
}

// ==============================================================================
void ThemeEditor::load_from_file(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) {
        ARX_LOG_WARN("ThemeEditor: no se pudo abrir '{}'", path);
        return;
    }
    std::stringstream ss;
    ss << f.rdbuf();
    std::string content = ss.str();
    f.close();

    // Parser JSON muy simple (regex-based)
    ImGuiStyle& s = ImGui::GetStyle();
    std::regex col_re("\"([^\"]+)\":\\s*\"#([0-9A-Fa-f]{8})\"");
    auto begin = std::sregex_iterator(content.begin(), content.end(), col_re);
    auto end = std::sregex_iterator();
    for (auto it = begin; it != end; ++it) {
        std::string name = (*it)[1].str();
        std::string hex = "#" + (*it)[2].str();
        // Buscar el ImGuiCol por nombre
        for (int i = 0; i < ImGuiCol_COUNT; ++i) {
            if (name == ImGui::GetStyleColorName(i)) {
                s.Colors[i] = hex_to_col(hex);
                break;
            }
        }
    }
    ARX_LOG_INFO("ThemeEditor: tema cargado desde '{}'", path);
}

} // namespace arx
