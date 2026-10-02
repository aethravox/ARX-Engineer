// ==============================================================================
// src/editor/plugins/editor_plugin.cpp
// ==============================================================================
#include "editor_plugin.hpp"

namespace arx {

void EditorPlugin::add_dock(Control* /*dock*/, const std::string& /*name*/) {
    // En una impl completa: docks_->add(dock, name);
}

void EditorPlugin::remove_dock(Control* /*dock*/) {}

void EditorPlugin::add_menu_item(const std::string& /*path*/,
                                   std::function<void()> /*callback*/) {}
void EditorPlugin::remove_menu_item(const std::string& /*path*/) {}

void EditorPlugin::add_inspector_plugin(InspectorPlugin* /*plugin*/) {}
void EditorPlugin::remove_inspector_plugin(InspectorPlugin* /*plugin*/) {}

void EditorPlugin::add_toolbar_button(const std::string& /*icon*/,
                                        std::function<void()> /*callback*/) {}

// ===================== PluginRegistry ========================================
PluginRegistry& PluginRegistry::instance() {
    static PluginRegistry r;
    return r;
}

void PluginRegistry::register_plugin(std::shared_ptr<EditorPlugin> p) {
    plugins_.push_back(std::move(p));
    if (plugins_.back()) plugins_.back()->_enter_tree();
}

void PluginRegistry::unregister_plugin(const std::string& name) {
    plugins_.erase(
        std::remove_if(plugins_.begin(), plugins_.end(),
            [&](const std::shared_ptr<EditorPlugin>& p) {
                if (p && std::string(p->get_class_name()) == name) {
                    p->_exit_tree();
                    return true;
                }
                return false;
            }),
        plugins_.end());
}

std::vector<EditorPlugin*> PluginRegistry::get_plugins() const {
    std::vector<EditorPlugin*> out;
    for (const auto& p : plugins_) out.push_back(p.get());
    return out;
}

void PluginRegistry::notify_scene_changed(Vox* root) {
    for (auto& p : plugins_) p->on_scene_changed(root);
}

void PluginRegistry::notify_node_selected(Vox* n) {
    for (auto& p : plugins_) p->on_node_selected(n);
}

void PluginRegistry::process_all(float delta) {
    for (auto& p : plugins_) p->process(delta);
}

} // namespace arx
