// ==============================================================================
// src/scene/ui/control.hpp — Base de nodos UI (estilo Control de Godot).
// ==============================================================================
#pragma once

#include "scene/vox.hpp"
#include "core/types.hpp"

namespace arx {

// Control — base de todos los widgets UI.
// Tiene posición + tamaño (rect), anclas (anchors), márgenes (offsets),
// y propagación de input/mouse.
class Control : public Vox {
public:
    ARX_CLASS(Control, Vox);
public:

    enum class Anchor {
        TopLeft, TopCenter, TopRight,
        CenterLeft, Center, CenterRight,
        BottomLeft, BottomCenter, BottomRight,
        FullRect, Custom
    };

    void set_position(Vector2 p)  { position_ = p; }
    void set_size(Vector2 s)      { size_ = s; }
    void set_anchor(Anchor a)     { anchor_ = a; }
    void set_margin_left(float v)   { margin_left_   = v; }
    void set_margin_top(float v)    { margin_top_    = v; }
    void set_margin_right(float v)  { margin_right_  = v; }
    void set_margin_bottom(float v) { margin_bottom_ = v; }
    void set_modulate(Color c)    { modulate_ = c; }
    void set_visible(bool v)      { Control::set_visible(v); }

    Vector2 get_position() const { return position_; }
    Vector2 get_size()     const { return size_; }
    Rect2   get_rect()     const { return Rect2(position_, size_); }
    Color   get_modulate() const { return modulate_; }
    Anchor  get_anchor()   const { return anchor_; }

    // Hit testing.
    virtual bool contains_point(Vector2 p) const {
        return get_rect().contains(p);
    }

    // Layout — overridear en containers (VBox, HBox, Grid).
    virtual void layout_children() {}

    // Input UI — se llama cuando hay un evento de mouse sobre este control.
    virtual void on_mouse_enter() {}
    virtual void on_mouse_exit()  {}
    virtual void on_mouse_move(Vector2 /*p*/) {}
    virtual void on_mouse_button(Vector2 /*p*/, int /*b*/, bool /*pressed*/) {}
    virtual void on_key(int /*k*/, bool /*pressed*/) {}

    // Focus.
    void set_focus_mode(int m) { focus_mode_ = m; }
    int  get_focus_mode() const { return focus_mode_; }
    bool has_focus() const { return has_focus_; }
    void grab_focus();
    void release_focus();

protected:
    Vector2 position_{0,0};
    Vector2 size_{100, 100};
    Anchor  anchor_ = Anchor::TopLeft;
    float   margin_left_   = 0.0f;
    float   margin_top_    = 0.0f;
    float   margin_right_  = 0.0f;
    float   margin_bottom_ = 0.0f;
    Color   modulate_  = Color::white;
    int     focus_mode_ = 0;     // 0=none, 1=click, 2=all
    bool    has_focus_  = false;
    bool    hovered_    = false;
};

// FocusManager — singleton que controla qué Control tiene el foco.
class FocusManager {
public:
    static FocusManager& instance();
    void set_focused(Control* c);
    Control* get_focused() const { return focused_; }
    void focus_next();
    void focus_prev();
    void register_control(Control* c);
    void unregister_control(Control* c);

private:
    FocusManager() = default;
    Control* focused_ = nullptr;
    std::vector<Control*> focusable_;
};

} // namespace arx
