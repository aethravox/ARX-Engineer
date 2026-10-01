// ==============================================================================
// src/theme/theme.cpp
// ==============================================================================
#include "theme.hpp"
#include "scene/ui/control.hpp"

namespace arx {

std::shared_ptr<Theme> Theme::create_preset(Preset p) {
    auto t = std::make_shared<Theme>();
    switch (p) {
        case Preset::Default:
        case Preset::Dark:
            t->set_color("background",   Color(0.13f, 0.13f, 0.16f, 1.0f));
            t->set_color("foreground",   Color(0.89f, 0.89f, 0.92f, 1.0f));
            t->set_color("accent",       Color(0.42f, 0.20f, 0.85f, 1.0f));
            t->set_color("disabled",     Color(0.40f, 0.40f, 0.45f, 1.0f));
            t->set_color("border",       Color(0.22f, 0.22f, 0.27f, 1.0f));
            t->set_font_size("normal", 14);
            t->set_font_size("title",  20);
            t->set_font_size("small",  12);
            break;
        case Preset::Light:
            t->set_color("background",   Color(0.95f, 0.95f, 0.97f, 1.0f));
            t->set_color("foreground",   Color(0.10f, 0.10f, 0.13f, 1.0f));
            t->set_color("accent",       Color(0.42f, 0.20f, 0.85f, 1.0f));
            t->set_color("disabled",     Color(0.60f, 0.60f, 0.65f, 1.0f));
            t->set_color("border",       Color(0.78f, 0.78f, 0.82f, 1.0f));
            break;
        case Preset::ARX:
            t->set_color("background",   Color(0.05f, 0.05f, 0.08f, 1.0f));
            t->set_color("foreground",   Color(0.95f, 0.95f, 0.98f, 1.0f));
            t->set_color("accent",       Color::arx_purple);
            t->set_color("accent_alt",   Color::arx_cyan);
            t->set_color("disabled",     Color(0.30f, 0.30f, 0.35f, 1.0f));
            t->set_color("border",       Color(0.20f, 0.20f, 0.25f, 1.0f));
            t->set_color("hover",        Color(0.15f, 0.15f, 0.20f, 1.0f));
            t->set_font_size("normal", 14);
            t->set_font_size("title",  22);
            t->set_font_size("small",  11);
            t->set_constant("margin",   8);
            t->set_constant("padding",  4);
            t->set_constant("radius",   4);
            break;
        case Preset::Cyberpunk:
            t->set_color("background",   Color(0.02f, 0.02f, 0.05f, 1.0f));
            t->set_color("foreground",   Color(1.00f, 0.10f, 0.70f, 1.0f));
            t->set_color("accent",       Color(0.10f, 1.00f, 0.92f, 1.0f));
            t->set_color("border",       Color(1.00f, 0.10f, 0.70f, 0.5f));
            break;
        case Preset::Retro:
            t->set_color("background",   Color(0.13f, 0.16f, 0.10f, 1.0f));
            t->set_color("foreground",   Color(0.60f, 1.00f, 0.40f, 1.0f));
            t->set_color("accent",       Color(1.00f, 0.80f, 0.20f, 1.0f));
            break;
    }
    return t;
}

void Theme::apply_to(Control* /*c*/) const {
    // En una implementación completa, aplicar colors/font_sizes a cada widget.
    // Por ahora es un stub.
}

} // namespace arx
