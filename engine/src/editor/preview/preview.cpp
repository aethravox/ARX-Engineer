// ==============================================================================
#include "core/variant.hpp"
// src/editor/preview/preview.cpp
// ==============================================================================
#include "preview.hpp"
#include "scene/scene_tree.hpp"
#include "core/logging.hpp"

#include <chrono>
#include <thread>
#include <algorithm>

namespace arx {

EditorPreview::EditorPreview() = default;
EditorPreview::~EditorPreview() { shutdown(); }

bool EditorPreview::init(Renderer* r, Window* w) {
    renderer_ = r;
    window_   = w;
    scene_tree_ = std::make_unique<SceneTree>();
    scene_tree_->init(w, r);
    return true;
}

void EditorPreview::shutdown() {
    stop();
    scene_tree_.reset();
}

void EditorPreview::play() {
    if (state_ == State::Playing) return;
    if (state_ == State::Stopped) {
        // Setup: clonar el edited_scene_ al scene_tree_.
        // Stub: en una impl completa, deep-copy del Vox tree.
    }
    update_state(State::Playing);
    running_ = true;
    // El loop se ejecuta en el thread principal (no en uno separado) para
    // no pelear con OpenGL context.
}

void EditorPreview::pause() {
    if (state_ == State::Playing) update_state(State::Paused);
}

void EditorPreview::resume() {
    if (state_ == State::Paused) update_state(State::Playing);
}

void EditorPreview::stop() {
    if (state_ == State::Stopped) return;
    running_ = false;
    update_state(State::Stopped);
    frame_count_ = 0;
}

void EditorPreview::step_one_frame() {
    if (state_ != State::Paused) return;
    update_state(State::Stepping);
    // Ejecutar un solo frame y volver a pausar.
    if (scene_tree_) scene_tree_->process(1.0f / 60.0f);
    update_state(State::Paused);
    ++frame_count_;
}

Vox* EditorPreview::get_edited_scene() const {
    return edited_scene_;
}

void EditorPreview::set_edited_scene(Vox* root) {
    edited_scene_ = root;
}

void EditorPreview::reload_script(const std::string& path) {
    ARX_LOG_INFO("Hot reload: {}", path);
    // En una impl completa: re-lex, re-parse, re-cargar en la VM,
    // y re-bind de los métodos override.
}

Variant EditorPreview::get_runtime_property(Vox* node, const std::string& prop) {
    if (!node) return {};
    return node->get_property(sid(prop.c_str()));
}

void EditorPreview::set_runtime_property(Vox* node, const std::string& prop, Variant v) {
    if (!node) return;
    node->set_property(sid(prop.c_str()), v);
}

void EditorPreview::lock()   { /* mutex_.lock(); */ }
void EditorPreview::unlock() { /* mutex_.unlock(); */ }

void EditorPreview::update_state(State new_state) {
    if (state_ == new_state) return;
    state_ = new_state;
    if (on_state_changed_) on_state_changed_(new_state);
}

// ===================== LiveEdit ==============================================
LiveEdit& LiveEdit::instance() {
    static LiveEdit l;
    return l;
}

Vox* LiveEdit::instantiate_node(const std::string& class_name, const std::string& name) {
    Object* obj = ClassDB::instance().instantiate(sid(class_name.c_str()));
    Vox* n = dynamic_cast<Vox*>(obj);
    if (n) n->set_name(name);
    return n;
}

void LiveEdit::remove_node(Vox* node) {
    if (node) node->remove_from_parent();
    delete node;
}

void LiveEdit::reparent_node(Vox* node, Vox* new_parent) {
    if (!node || !new_parent) return;
    node->remove_from_parent();
    new_parent->add_child(node);
}

void LiveEdit::set_property(Vox* node, const std::string& property, Variant value) {
    if (node) node->set_property(sid(property.c_str()), value);
}

Variant LiveEdit::get_property(Vox* node, const std::string& property) {
    return node ? node->get_property(sid(property.c_str())) : Variant{};
}

Variant LiveEdit::call_method(Vox* node, const std::string& method,
                                const std::vector<Variant>& args) {
    if (!node) return {};
    return node->call_method(sid(method.c_str()), args);
}

void LiveEdit::watch_expression(const std::string& expr,
                                  std::function<void(Variant)> cb) {
    watches_.emplace_back(expr, std::move(cb));
}

void LiveEdit::clear_watches() { watches_.clear(); }

void LiveEdit::update_watches() {
    // Stub: en una impl completa se evaluaría cada expresión en el contexto
    // del SceneTree actual y se llamaría al callback con el nuevo valor.
}

} // namespace arx
