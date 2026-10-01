// ==============================================================================
// src/scene/ui/more_widgets.hpp — Más widgets UI.
// TreeView, TabContainer, GraphEdit, FileDialog, ColorPicker,
// SpinBox, OptionButton, TextEdit con syntax highlight, MenuItem, MenuButton,
// Range, Separator.
// ==============================================================================
#pragma once

#include "scene/ui/control.hpp"
#include "scene/ui/widgets.hpp"
#include "render/texture.hpp"

#include <vector>
#include <string>
#include <functional>

namespace arx {

// TreeView — árbol expandible (como el Scene Tree dock).
class TreeView : public Control {
public:
    ARX_CLASS(TreeView, Control);
public:

    struct Item {
        std::string text;
        std::shared_ptr<Texture> icon;
        std::vector<Item> children;
        bool expanded = false;
        bool selected = false;
        std::string metadata;
    };

    void set_root(Item r) { root_ = std::move(r); }
    Item& get_root() { return root_; }
    void clear() { root_ = {}; }

    void on_item_selected(std::function<void(const std::string&)> cb) {
        on_selected_ = std::move(cb);
    }

private:
    Item root_;
    std::function<void(const std::string&)> on_selected_;
};

// TabContainer — pestañas.
class TabContainer : public Control {
public:
    ARX_CLASS(TabContainer, Control);
public:

    int  add_tab(const std::string& title, Control* content);
    void remove_tab(int idx);
    void set_current_tab(int idx) { current_ = std::clamp(idx, 0, (int)tabs_.size()-1); }
    int  get_current_tab() const { return current_; }
    int  get_tab_count() const   { return (int)tabs_.size(); }
    std::string get_tab_title(int idx) const {
        return (idx >= 0 && idx < (int)tabs_.size()) ? tabs_[idx].first : "";
    }

private:
    std::vector<std::pair<std::string, Control*>> tabs_;
    int current_ = 0;
};

// GraphEdit — canvas para visual scripting (estilo Godot GraphEdit).
class GraphEdit : public Control {
public:
    ARX_CLASS(GraphEdit, Control);
public:

    void set_zoom(float z) { zoom_ = std::clamp(z, 0.1f, 4.0f); }
    float get_zoom() const { return zoom_; }
    void set_scroll_offset(Vector2 o) { scroll_ = o; }
    Vector2 get_scroll_offset() const { return scroll_; }

    void on_node_selected(std::function<void(uint32_t)> cb) { on_node_sel_ = std::move(cb); }
    void on_connection_request(std::function<void(uint32_t, uint32_t)> cb) {
        on_conn_req_ = std::move(cb);
    }

private:
    float zoom_ = 1.0f;
    Vector2 scroll_{0, 0};
    std::function<void(uint32_t)> on_node_sel_;
    std::function<void(uint32_t, uint32_t)> on_conn_req_;
};

// FileDialog — diálogo abrir/guardar archivo.
class FileDialog : public Control {
public:
    ARX_CLASS(FileDialog, Control);
public:

    enum class Mode { OpenFile, SaveFile, OpenDir };

    void set_mode(Mode m) { mode_ = m; }
    void set_filters(const std::vector<std::string>& f) { filters_ = f; }
    void set_current_dir(const std::string& d) { current_dir_ = d; }

    void on_file_selected(std::function<void(const std::string&)> cb) {
        on_selected_ = std::move(cb);
    }

    void popup() { visible_ = true; }

private:
    Mode mode_ = Mode::OpenFile;
    std::vector<std::string> filters_;
    std::string current_dir_ = ".";
    std::function<void(const std::string&)> on_selected_;
};

// ColorPicker — picker HSV + RGB + hex.
class ColorPicker : public Control {
public:
    ARX_CLASS(ColorPicker, Control);
public:

    void set_color(Color c) { color_ = c; }
    Color get_color() const { return color_; }
    void set_hsv_mode(bool h) { hsv_mode_ = h; }
    void set_alpha_enabled(bool a) { alpha_enabled_ = a; }

    void on_color_changed(std::function<void(Color)> cb) { on_changed_ = std::move(cb); }

private:
    Color color_ = Color::white;
    bool hsv_mode_ = false;
    bool alpha_enabled_ = true;
    std::function<void(Color)> on_changed_;
};

// SpinBox — entrada numérica con +/- buttons.
class SpinBox : public Control {
public:
    ARX_CLASS(SpinBox, Control);
public:

    void set_min(float v) { min_ = v; }
    void set_max(float v) { max_ = v; }
    void set_value(float v);
    float get_value() const { return value_; }
    void set_step(float s) { step_ = s; }
    void set_suffix(const std::string& s) { suffix_ = s; }
    void set_prefix(const std::string& p) { prefix_ = p; }

    void on_value_changed(std::function<void(float)> cb) { on_changed_ = std::move(cb); }

private:
    float min_ = 0.0f, max_ = 100.0f, value_ = 0.0f, step_ = 1.0f;
    std::string suffix_, prefix_;
    std::function<void(float)> on_changed_;
};

// MenuButton — botón que despliega un menú.
class MenuButton : public Control {
public:
    ARX_CLASS(MenuButton, Control);
public:

    void set_text(const std::string& s) { text_ = s; }
    void add_item(const std::string& label, std::function<void()> cb);
    void clear_items();

private:
    struct Item { std::string label; std::function<void()> cb; };
    std::string text_;
    std::vector<Item> items_;
};

// Separator — línea separadora.
class Separator : public Control {
public:
    ARX_CLASS(Separator, Control);
public:
    void set_orientation(int o) { orientation_ = o; }  // 0=horizontal, 1=vertical
private:
    int orientation_ = 0;
};

// Range — base para controles de rango (Slider, ProgressBar, SpinBox).
class Range : public Control {
public:
    ARX_CLASS(Range, Control);
public:
    void set_min(float v) { min_ = v; }
    void set_max(float v) { max_ = v; }
    void set_value(float v) { value_ = std::clamp(v, min_, max_); }
    void set_step(float s) { step_ = s; }
    void set_rounded(bool r) { rounded_ = r; }

    float get_min() const { return min_; }
    float get_max() const { return max_; }
    float get_value() const { return value_; }
    float get_step() const { return step_; }

    float get_as_ratio() const {
        return (max_ > min_) ? (value_ - min_) / (max_ - min_) : 0.0f;
    }

protected:
    float min_ = 0.0f, max_ = 1.0f, value_ = 0.0f, step_ = 0.01f;
    bool  rounded_ = false;
};

} // namespace arx
