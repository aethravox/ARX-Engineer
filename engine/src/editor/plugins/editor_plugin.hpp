// ==============================================================================
// src/editor/plugins/editor_plugin.hpp — Sistema de plugins del editor.
// ==============================================================================
#pragma once

#include "core/types.hpp"
#include "core/object.hpp"

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace arx {

class Vox;
class Control;
class EditorMain;

class EditorPlugin : public Object {
public:
    ARX_CLASS(EditorPlugin, Object);
public:

    virtual void _enter_tree()    {}
    virtual void _exit_tree()     {}
    virtual void process(float)  {}
    virtual void on_scene_changed(Vox* /*root*/) {}
    virtual void on_node_selected(Vox* /*n*/) {}

    void add_dock(Control* dock, const std::string& name);
    void remove_dock(Control* dock);

    void add_menu_item(const std::string& path, std::function<void()> callback);
    void remove_menu_item(const std::string& path);

    void add_inspector_plugin(class InspectorPlugin* plugin);
    void remove_inspector_plugin(InspectorPlugin* plugin);

    void add_toolbar_button(const std::string& icon,
                              std::function<void()> callback);

    void set_editor(EditorMain* e) { editor_ = e; }
    EditorMain* get_editor() const { return editor_; }

private:
    EditorMain* editor_ = nullptr;
};

class InspectorPlugin : public Object {
public:
    ARX_CLASS(InspectorPlugin, Object);
public:

    virtual bool can_handle(Object* obj) const = 0;
    virtual Control* create_editor(Object* obj) = 0;
};

class PluginRegistry {
public:
    static PluginRegistry& instance();

    void register_plugin(std::shared_ptr<EditorPlugin> p);
    void unregister_plugin(const std::string& name);
    std::vector<EditorPlugin*> get_plugins() const;

    void notify_scene_changed(Vox* root);
    void notify_node_selected(Vox* n);
    void process_all(float delta);

private:
    PluginRegistry() = default;
    std::vector<std::shared_ptr<EditorPlugin>> plugins_;
};

#define ARX_PLUGIN(PluginClass) \
    static const bool _arx_plugin_##PluginClass = []() { \
        ::arx::PluginRegistry::instance().register_plugin( \
            std::make_shared<PluginClass>()); \
        return true; \
    }();

} // namespace arx
