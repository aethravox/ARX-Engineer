// ==============================================================================
// src/scene/ui/ui_nodes.hpp — Nodos de UI para apps y games.
// VBox, HBox, Button, Label, ColorRect, Spacer, Image, ProgressBar, Slider.
// ==============================================================================
#pragma once

#include "scene/2d/vox2d.hpp"
#include "render/texture.hpp"
#include "core/variant.hpp"

#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace arx::ui {

// ==============================================================================
// Control: base de widgets UI (similar a Godot Control).
// ==============================================================================
class Control : public Vox2D {
public:
    ARX_CLASS(Control, Vox2D);
public:

    enum class Anchor {
        TopLeft, TopCenter, TopRight,
        CenterLeft, Center, CenterRight,
        BottomLeft, BottomCenter, BottomRight,
        FullRect
    };

    void set_anchor(Anchor a) { anchor_ = a; dirty_layout_ = true; }
    Anchor get_anchor() const { return anchor_; }

    void set_margin(float left, float top, float right, float bottom) {
        margins_ = {left, top, right, bottom};
        dirty_layout_ = true;
    }
    Vector4 get_margins() const { return margins_; }

    void set_size(Vector2 s) { size_ = s; }
    Vector2 get_size() const { return size_; }

    void set_visible(bool v) override { visible_ = v; }

    // Mouse events.
    virtual void on_mouse_enter() {}
    virtual void on_mouse_exit()  {}
    virtual void on_gui_input(const InputEvent&) {}

protected:
    Anchor   anchor_       = Anchor::TopLeft;
    Vector4  margins_      = {0,0,0,0};   // left, top, right, bottom
    Vector2  size_         = {100, 30};
    bool     dirty_layout_ = true;
};

// ==============================================================================
// BoxContainer: base para VBox y HBox. Layout de hijos con spacing + padding.
// ==============================================================================
class BoxContainer : public Control {
public:
    ARX_CLASS(BoxContainer, Control);
public:

    void set_spacing(float s) { spacing_ = s; dirty_layout_ = true; }
    float get_spacing() const { return spacing_; }

    void set_padding(float all) { padding_left_ = padding_right_ = padding_top_ = padding_bottom_ = all; dirty_layout_ = true; }
    void set_padding(float h, float v) { padding_left_ = padding_right_ = h; padding_top_ = padding_bottom_ = v; dirty_layout_ = true; }

    enum class Direction { Vertical, Horizontal };

protected:
    BoxContainer(Direction d) : direction_(d) {}

    void layout_children_() {
        if (!dirty_layout_) return;
        dirty_layout_ = false;

        auto children = get_children();
        float offset = (direction_ == Direction::Vertical)
            ? padding_top_ : padding_left_;
        for (auto* c : children) {
            auto* ctrl = dynamic_cast<Control*>(c);
            if (!ctrl) continue;
            Vector2 child_size = ctrl->get_size();
            if (direction_ == Direction::Vertical) {
                ctrl->set_position({padding_left_, offset});
                offset += child_size.y + spacing_;
            } else {
                ctrl->set_position({offset, padding_top_});
                offset += child_size.x + spacing_;
            }
        }
        // Actualizar tamaño propio.
        if (direction_ == Direction::Vertical) {
            size_.y = offset + padding_bottom_;
        } else {
            size_.x = offset + padding_right_;
        }
    }

    Direction direction_;
    float spacing_         = 4.0f;
    float padding_left_    = 4.0f;
    float padding_right_   = 4.0f;
    float padding_top_     = 4.0f;
    float padding_bottom_  = 4.0f;
};

// ==============================================================================
// VBox — layout vertical (de arriba a abajo).
// ==============================================================================
class VBox : public BoxContainer {
public:
    ARX_CLASS(VBox, BoxContainer);
public:
    VBox() : BoxContainer(Direction::Vertical) {}

    void process(float) override { layout_children_(); }
};

// ==============================================================================
// HBox — layout horizontal (de izquierda a derecha).
// ==============================================================================
class HBox : public BoxContainer {
public:
    ARX_CLASS(HBox, BoxContainer);
public:
    HBox() : BoxContainer(Direction::Horizontal) {}

    void process(float) override { layout_children_(); }
};

// ==============================================================================
// Label — texto no editable.
// ==============================================================================
class Label : public Control {
public:
    ARX_CLASS(Label, Control);
public:

    void set_text(const std::string& s) { text_ = s; }
    const std::string& get_text() const { return text_; }
    void set_color(Color c) { color_ = c; }
    Color get_color() const { return color_; }
    void set_font_size(int s) { font_size_ = s; }
    int get_font_size() const { return font_size_; }
    void set_alignment(int a) { alignment_ = a; }  // 0=left,1=center,2=right

private:
    std::string text_;
    Color       color_     = Color::white;
    int         font_size_ = 16;
    int         alignment_ = 0;
};

// ==============================================================================
// Button — botón clickeable.
// ==============================================================================
class Button : public Control {
public:
    ARX_CLASS(Button, Control);
public:

    Button() { size_ = {100, 30}; }

    void set_text(const std::string& t) { text_ = t; }
    const std::string& get_text() const { return text_; }

    void set_disabled(bool d) { disabled_ = d; }
    bool is_disabled() const { return disabled_; }

    void set_pressed(bool p) { pressed_ = p; }
    bool is_pressed() const { return pressed_; }

    void set_toggle_mode(bool t) { toggle_mode_ = t; }
    bool is_toggle_mode() const { return toggle_mode_; }

    // Signal: emit when clicked.
    void emit_pressed() {
        emit_signal(sid("pressed"));
    }
    void emit_released() {
        emit_signal(sid("released"));
    }

    void on_gui_input(const InputEvent& ev) override {
        if (disabled_) return;
        if (ev.kind == InputEvent::Kind::MouseButton && ev.mouse_button == MouseButton::Left) {
            if (ev.pressed) {
                pressed_ = true;
                emit_pressed();
            } else if (ev.released) {
                if (toggle_mode_) pressed_ = !pressed_;
                else              pressed_ = false;
                emit_released();
            }
        }
    }

private:
    std::string text_;
    bool        disabled_    = false;
    bool        pressed_     = false;
    bool        toggle_mode_ = false;
};

// ==============================================================================
// ColorRect — rectángulo relleno de un color (para backgrounds).
// ==============================================================================
class ColorRect : public Control {
public:
    ARX_CLASS(ColorRect, Control);
public:

    void set_color(Color c) { color_ = c; }
    Color get_color() const { return color_; }

private:
    Color color_ = Color::gray;
};

// ==============================================================================
// Spacer — espacio vacío con tamaño fijo (para BoxContainer).
// ==============================================================================
class Spacer : public Control {
public:
    ARX_CLASS(Spacer, Control);
public:
    Spacer() { size_ = {10, 10}; }

    void set_height(float h) { size_.y = h; }
    void set_width(float w)  { size_.x = w; }
};

// ==============================================================================
// Image — muestra una textura.
// ==============================================================================
class Image : public Control {
public:
    ARX_CLASS(Image, Control);
public:

    void set_texture(std::shared_ptr<Texture> tex) { texture_ = tex; }
    std::shared_ptr<Texture> get_texture() const { return texture_; }

    void set_stretch(bool s) { stretch_ = s; }
    bool is_stretch() const { return stretch_; }

    void set_flip_h(bool f) { flip_h_ = f; }
    void set_flip_v(bool f) { flip_v_ = f; }

private:
    std::shared_ptr<Texture> texture_;
    bool stretch_ = true;
    bool flip_h_  = false;
    bool flip_v_  = false;
};

// ==============================================================================
// ProgressBar — barra de progreso.
// ==============================================================================
class ProgressBar : public Control {
public:
    ARX_CLASS(ProgressBar, Control);
public:

    ProgressBar() { size_ = {200, 20}; }

    void set_value(float v) { value_ = std::clamp(v, 0.0f, 1.0f); }
    float get_value() const { return value_; }

    void set_color(Color c) { color_ = c; }
    Color get_color() const { return color_; }

    void set_bg_color(Color c) { bg_color_ = c; }
    Color get_bg_color() const { return bg_color_; }

private:
    float value_     = 0.0f;
    Color color_     = Color::arx_purple;
    Color bg_color_  = Color::gray * 0.5f;
};

// ==============================================================================
// Slider — slider horizontal o vertical.
// ==============================================================================
class Slider : public Control {
public:
    ARX_CLASS(Slider, Control);
public:

    enum class Orientation { Horizontal, Vertical };

    Slider() { size_ = {200, 20}; }

    void set_value(float v) { value_ = std::clamp(v, min_, max_); }
    float get_value() const { return value_; }
    void set_min(float m) { min_ = m; }
    void set_max(float m) { max_ = m; }
    void set_step(float s) { step_ = s; }
    void set_orientation(Orientation o) { orientation_ = o; }

private:
    float       value_       = 0.0f;
    float       min_         = 0.0f;
    float       max_         = 100.0f;
    float       step_        = 1.0f;
    Orientation orientation_ = Orientation::Horizontal;
};

// ==============================================================================
// LineEdit — input de texto de una línea.
// ==============================================================================
class LineEdit : public Control {
public:
    ARX_CLASS(LineEdit, Control);
public:

    LineEdit() { size_ = {200, 24}; }

    void set_text(const std::string& t) { text_ = t; }
    const std::string& get_text() const { return text_; }

    void set_placeholder(const std::string& p) { placeholder_ = p; }
    void set_max_length(int l) { max_length_ = l; }
    void set_secret(bool s) { secret_ = s; }
    void set_editable(bool e) { editable_ = e; }

    // Signals: text_changed, text_submitted (Enter)

private:
    std::string text_;
    std::string placeholder_ = "Enter text...";
    int         max_length_  = 0;
    bool        secret_      = false;
    bool        editable_    = true;
    bool        focused_     = false;
    int         cursor_pos_  = 0;
};

// ==============================================================================
// CheckBox — toggle con label.
// ==============================================================================
class CheckBox : public Control {
public:
    ARX_CLASS(CheckBox, Control);
public:

    CheckBox() { size_ = {120, 24}; }

    void set_checked(bool c) { checked_ = c; }
    bool is_checked() const { return checked_; }

    void set_label(const std::string& l) { label_ = l; }
    const std::string& get_label() const { return label_; }

private:
    bool        checked_ = false;
    std::string label_;
};

} // namespace arx::ui
