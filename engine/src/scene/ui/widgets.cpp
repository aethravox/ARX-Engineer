// ==============================================================================
// src/scene/ui/widgets.cpp
// ==============================================================================
#include "widgets.hpp"
#include "core/math.hpp"
#include "input/input.hpp"

namespace arx {

// ===================== LineEdit ==============================================
void LineEdit::on_key(int k, bool pressed) {
    if (readonly_ || !pressed) return;
    if (k == (int)Key::Backspace && !text_.empty()) {
        text_.pop_back();
        if (on_text_changed_) on_text_changed_(text_);
    } else if (k == (int)Key::Enter) {
        if (on_text_submitted_) on_text_submitted_(text_);
    }
    // Ingreso de caracteres lo maneja el sistema de char events del OS.
}

// ===================== Slider ================================================
void Slider::set_value(float v) {
    v = std::clamp(v, min_, max_);
    if (step_ > 0) v = std::round(v / step_) * step_;
    if (v != value_) {
        value_ = v;
        if (on_value_changed_) on_value_changed_(value_);
    }
}

// ===================== VBox ==================================================
void VBox::layout_children() {
    float y = position_.y + padding_;
    float w = size_.x - padding_ * 2;
    for (auto* c : get_children()) {
        auto* ctrl = dynamic_cast<Control*>(c);
        if (!ctrl) continue;
        Vector2 cs = ctrl->get_size();
        ctrl->set_position({ position_.x + padding_, y });
        // Mantener el tamaño del hijo si lo tiene, sino usar w.
        if (cs.x <= 0) ctrl->set_size({ w, cs.y });
        y += ctrl->get_size().y + spacing_;
    }
}

// ===================== HBox ==================================================
void HBox::layout_children() {
    float x = position_.x + padding_;
    float h = size_.y - padding_ * 2;
    for (auto* c : get_children()) {
        auto* ctrl = dynamic_cast<Control*>(c);
        if (!ctrl) continue;
        Vector2 cs = ctrl->get_size();
        ctrl->set_position({ x, position_.y + padding_ });
        if (cs.y <= 0) ctrl->set_size({ cs.x, h });
        x += ctrl->get_size().x + spacing_;
    }
}

// ===================== Grid ==================================================
void Grid::layout_children() {
    float x = position_.x + padding_;
    float y = position_.y + padding_;
    int col = 0;
    float max_row_h = 0;
    float col_w = (size_.x - padding_ * 2 - spacing_ * (columns_ - 1)) / columns_;
    for (auto* c : get_children()) {
        auto* ctrl = dynamic_cast<Control*>(c);
        if (!ctrl) continue;
        ctrl->set_position({ x, y });
        ctrl->set_size({ col_w, ctrl->get_size().y });
        max_row_h = std::max(max_row_h, ctrl->get_size().y);
        ++col;
        if (col >= columns_) {
            col = 0;
            x = position_.x + padding_;
            y += max_row_h + spacing_;
            max_row_h = 0;
        } else {
            x += col_w + spacing_;
        }
    }
}

} // namespace arx
