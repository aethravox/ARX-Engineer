// ==============================================================================
// src/scene/ui/widgets.hpp — Widgets UI concretos.
// Button, Label, ColorRect, Spacer, LineEdit, Slider, ProgressBar,
// CheckBox, Image, VBox, HBox, Grid, ScrollContainer.
// ==============================================================================
#pragma once

#include "scene/ui/control.hpp"
#include "render/texture.hpp"

#include <string>
#include <functional>
#include <vector>

namespace arx {

// ---- Label -----------------------------------------------------------------
class Label : public Control {
public:
    ARX_CLASS(Label, Control);
public:

    void set_text(const std::string& s) { text_ = s; }
    const std::string& get_text() const { return text_; }
    void set_color(Color c)  { color_ = c; }
    Color get_color() const  { return color_; }
    void set_font_size(int s){ font_size_ = s; }
    int  get_font_size() const { return font_size_; }
    void set_alignment(int a) { alignment_ = a; }   // 0=left, 1=center, 2=right
    void set_autowrap(bool v) { autowrap_ = v; }

private:
    std::string text_;
    Color       color_      = Color::white;
    int         font_size_  = 16;
    int         alignment_  = 0;
    bool        autowrap_   = false;
};

// ---- ColorRect --------------------------------------------------------------
class ColorRect : public Control {
public:
    ARX_CLASS(ColorRect, Control);
public:

    void set_color(Color c) { color_ = c; }
    Color get_color() const { return color_; }

private:
    Color color_ = Color(0.2f, 0.2f, 0.2f, 1.0f);
};

// ---- Image ------------------------------------------------------------------
class Image : public Control {
public:
    ARX_CLASS(Image, Control);
public:

    void set_texture(std::shared_ptr<Texture> t) { texture_ = t; }
    std::shared_ptr<Texture> get_texture() const { return texture_; }
    void set_stretch(bool s) { stretch_ = s; }
    void set_flip_h(bool f)  { flip_h_ = f; }
    void set_flip_v(bool f)  { flip_v_ = f; }

private:
    std::shared_ptr<Texture> texture_;
    bool stretch_ = true;
    bool flip_h_  = false;
    bool flip_v_  = false;
};

// ---- Spacer -----------------------------------------------------------------
class Spacer : public Control {
public:
    ARX_CLASS(Spacer, Control);
public:
};

// ---- Button -----------------------------------------------------------------
class Button : public Control {
public:
    ARX_CLASS(Button, Control);
public:

    void set_text(const std::string& s) { text_ = s; }
    const std::string& get_text() const { return text_; }
    void set_disabled(bool d) { disabled_ = d; }
    bool is_disabled() const { return disabled_; }
    void set_toggle_mode(bool t) { toggle_mode_ = t; }
    bool is_pressed() const { return pressed_; }
    void set_pressed(bool p) { pressed_ = p; }

    // Signals (estilo Godot).
    void on_pressed(std::function<void()> cb) { on_pressed_ = std::move(cb); }
    void on_toggled(std::function<void(bool)> cb) { on_toggled_ = std::move(cb); }

    // Input handling.
    void on_mouse_button(Vector2, int b, bool pressed) override {
        if (disabled_) return;
        if (b == 0 && pressed) {
            if (toggle_mode_) { pressed_ = !pressed_; if (on_toggled_) on_toggled_(pressed_); }
            if (on_pressed_)  on_pressed_();
        }
    }

private:
    std::string text_;
    bool disabled_      = false;
    bool toggle_mode_   = false;
    bool pressed_       = false;
    std::function<void()>        on_pressed_;
    std::function<void(bool)>    on_toggled_;
};

// ---- LineEdit ---------------------------------------------------------------
class LineEdit : public Control {
public:
    ARX_CLASS(LineEdit, Control);
public:

    void set_text(const std::string& s) { text_ = s; }
    const std::string& get_text() const { return text_; }
    void set_placeholder(const std::string& s) { placeholder_ = s; }
    void set_max_length(int n) { max_length_ = n; }
    void set_secret(bool s) { secret_ = s; }
    void set_readonly(bool r) { readonly_ = r; }

    void on_text_changed(std::function<void(const std::string&)> cb) {
        on_text_changed_ = std::move(cb);
    }
    void on_text_submitted(std::function<void(const std::string&)> cb) {
        on_text_submitted_ = std::move(cb);
    }

    void on_key(int k, bool pressed) override;

private:
    std::string text_;
    std::string placeholder_;
    int         max_length_ = 0;
    bool        secret_     = false;
    bool        readonly_   = false;
    std::function<void(const std::string&)> on_text_changed_;
    std::function<void(const std::string&)> on_text_submitted_;
};

// ---- Slider -----------------------------------------------------------------
class Slider : public Control {
public:
    ARX_CLASS(Slider, Control);
public:

    void set_min(float v) { min_ = v; }
    void set_max(float v) { max_ = v; }
    void set_value(float v);
    float get_value() const { return value_; }
    void set_step(float s) { step_ = s; }
    void set_orientation(int o) { orientation_ = o; }  // 0=horizontal, 1=vertical

    void on_value_changed(std::function<void(float)> cb) {
        on_value_changed_ = std::move(cb);
    }

private:
    float min_ = 0.0f, max_ = 1.0f, value_ = 0.0f, step_ = 0.01f;
    int   orientation_ = 0;
    std::function<void(float)> on_value_changed_;
};

// ---- ProgressBar ------------------------------------------------------------
class ProgressBar : public Control {
public:
    ARX_CLASS(ProgressBar, Control);
public:

    void set_min(float v) { min_ = v; }
    void set_max(float v) { max_ = v; }
    void set_value(float v) { value_ = v; }
    float get_value() const { return value_; }
    void set_percentage_visible(bool v) { show_percentage_ = v; }

private:
    float min_ = 0.0f, max_ = 100.0f, value_ = 0.0f;
    bool  show_percentage_ = true;
};

// ---- CheckBox ---------------------------------------------------------------
class CheckBox : public Control {
public:
    ARX_CLASS(CheckBox, Control);
public:

    void set_checked(bool c) { checked_ = c; }
    bool is_checked() const { return checked_; }
    void set_text(const std::string& t) { text_ = t; }
    const std::string& get_text() const { return text_; }

    void on_toggled(std::function<void(bool)> cb) { on_toggled_ = std::move(cb); }
    void on_mouse_button(Vector2, int b, bool pressed) override {
        if (b == 0 && pressed) {
            checked_ = !checked_;
            if (on_toggled_) on_toggled_(checked_);
        }
    }

private:
    bool checked_ = false;
    std::string text_;
    std::function<void(bool)> on_toggled_;
};

// ---- Containers -------------------------------------------------------------
class Container : public Control {
public:
    ARX_CLASS(Container, Control);
public:

    void set_padding(float p) { padding_ = p; }
    void set_spacing(float s) { spacing_ = s; }
    float get_padding() const { return padding_; }
    float get_spacing() const { return spacing_; }

    void layout_children() override {}

protected:
    float padding_ = 8.0f;
    float spacing_ = 4.0f;
};

class VBox : public Container {
public:
    ARX_CLASS(VBox, Container);
public:
    void layout_children() override;
};

class HBox : public Container {
public:
    ARX_CLASS(HBox, Container);
public:
    void layout_children() override;
};

class Grid : public Container {
public:
    ARX_CLASS(Grid, Container);
public:

    void set_columns(int c) { columns_ = c; }
    int  get_columns() const { return columns_; }
    void layout_children() override;

private:
    int columns_ = 2;
};

class ScrollContainer : public Container {
public:
    ARX_CLASS(ScrollContainer, Container);
public:

    void set_scroll_horizontal(float v) { scroll_x_ = v; }
    void set_scroll_vertical(float v)   { scroll_y_ = v; }
    float get_scroll_horizontal() const { return scroll_x_; }
    float get_scroll_vertical()   const { return scroll_y_; }

private:
    float scroll_x_ = 0.0f;
    float scroll_y_ = 0.0f;
};

// ---- Window (panel flotante) ------------------------------------------------
class UIWindow : public Control {
public:
    ARX_CLASS(UIWindow, Control);
public:

    void set_title(const std::string& t) { title_ = t; }
    const std::string& get_title() const { return title_; }
    void set_closable(bool c) { closable_ = c; }
    void set_resizable(bool r) { resizable_ = r; }

private:
    std::string title_;
    bool closable_  = true;
    bool resizable_ = true;
};

// ---- OptionButton (dropdown) ------------------------------------------------
class OptionButton : public Control {
public:
    ARX_CLASS(OptionButton, Control);
public:

    void add_item(const std::string& s) { items_.push_back(s); }
    void clear() { items_.clear(); selected_ = 0; }
    void select(int idx) { selected_ = std::clamp(idx, 0, (int)items_.size()-1); }
    int  get_selected() const { return selected_; }
    std::string get_selected_text() const {
        return (selected_ >= 0 && selected_ < (int)items_.size())
               ? items_[selected_] : "";
    }
    const std::vector<std::string>& get_items() const { return items_; }

private:
    std::vector<std::string> items_;
    int selected_ = 0;
};

// ---- TextEdit (multiline text) ----------------------------------------------
class TextEdit : public Control {
public:
    ARX_CLASS(TextEdit, Control);
public:

    void set_text(const std::string& s) { text_ = s; }
    const std::string& get_text() const { return text_; }
    void set_readonly(bool r) { readonly_ = r; }
    void set_line_numbers(bool s) { show_line_numbers_ = s; }

private:
    std::string text_;
    bool readonly_ = false;
    bool show_line_numbers_ = false;
};

} // namespace arx
