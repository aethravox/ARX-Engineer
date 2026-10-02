// ==============================================================================
// src/scene/ui/more_widgets.cpp
// ==============================================================================
#include "more_widgets.hpp"

namespace arx {

// ===================== TabContainer ==========================================
int TabContainer::add_tab(const std::string& title, Control* content) {
    tabs_.emplace_back(title, content);
    return (int)tabs_.size() - 1;
}

void TabContainer::remove_tab(int idx) {
    if (idx < 0 || idx >= (int)tabs_.size()) return;
    tabs_.erase(tabs_.begin() + idx);
    if (current_ >= (int)tabs_.size()) current_ = (int)tabs_.size() - 1;
}

// ===================== SpinBox ===============================================
void SpinBox::set_value(float v) {
    v = std::clamp(v, min_, max_);
    if (step_ > 0) v = std::round(v / step_) * step_;
    if (v != value_) {
        value_ = v;
        if (on_changed_) on_changed_(value_);
    }
}

// ===================== MenuButton ============================================
void MenuButton::add_item(const std::string& label, std::function<void()> cb) {
    items_.push_back({label, std::move(cb)});
}

void MenuButton::clear_items() { items_.clear(); }

} // namespace arx
