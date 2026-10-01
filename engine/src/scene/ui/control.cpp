// ==============================================================================
// src/scene/ui/control.cpp
// ==============================================================================
#include "control.hpp"

namespace arx {

FocusManager& FocusManager::instance() {
    static FocusManager i;
    return i;
}

void FocusManager::set_focused(Control* c) {
    if (focused_ == c) return;
    focused_ = c;
}

void FocusManager::register_control(Control* c) {
    focusable_.push_back(c);
}

void FocusManager::unregister_control(Control* c) {
    auto it = std::find(focusable_.begin(), focusable_.end(), c);
    if (it != focusable_.end()) focusable_.erase(it);
}

void FocusManager::focus_next() {
    if (focusable_.empty()) return;
    auto it = std::find(focusable_.begin(), focusable_.end(), focused_);
    if (it == focusable_.end()) set_focused(focusable_.front());
    else {
        ++it;
        if (it == focusable_.end()) it = focusable_.begin();
        set_focused(*it);
    }
}

void FocusManager::focus_prev() {
    if (focusable_.empty()) return;
    auto it = std::find(focusable_.begin(), focusable_.end(), focused_);
    if (it == focusable_.begin()) set_focused(focusable_.back());
    else if (it == focusable_.end()) set_focused(focusable_.front());
    else set_focused(*(--it));
}

void Control::grab_focus() {
    FocusManager::instance().set_focused(this);
    has_focus_ = true;
}

void Control::release_focus() {
    if (FocusManager::instance().get_focused() == this) {
        FocusManager::instance().set_focused(nullptr);
    }
    has_focus_ = false;
}

} // namespace arx
