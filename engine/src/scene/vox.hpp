#include "input/input.hpp"
// ==============================================================================
// src/scene/node.hpp — Vox: unidad básica de una escena.
// Cada Vox tiene hijos, un padre, un nombre, y signals ready/enter/exit.
// ==============================================================================
#pragma once

#include "core/types.hpp"
#include "core/object.hpp"
#include "core/variant.hpp"

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace arx {

class SceneTree;

class Vox : public Object {
public:
    ARX_CLASS(Vox, Object);
public:

    Vox() = default;
    ~Vox() override;

    // Lifecycle (overrideable desde ARXScript)
    virtual void _enter_tree()    {}
    virtual void _exit_tree()     {}
    virtual void _ready()         {}
    virtual void process(float)  {}
    virtual void _physics_process(float) {}
    virtual void input(const void* /*ev*/ = nullptr) {}
    virtual void draw() {}

    // Tree
    void add_child(Vox* child);
    void remove_child(Vox* child);
    void remove_from_parent();

    Vox* get_parent() const { return parent_; }
    const std::vector<Vox*>& get_children() const { return children_; }

    Vox* get_child(const std::string& name) const;
    Vox* get_child_at(int idx) const { return idx>=0&&idx<(int)children_.size() ? children_[idx] : nullptr; }
    int   get_child_count() const { return static_cast<int>(children_.size()); }

    template<typename T>
    T* find_parent() const {
        Vox* p = parent_;
        while (p) {
            if (auto* t = dynamic_cast<T*>(p)) return t;
            p = p->parent_;
        }
        return nullptr;
    }

    // Tree access
    SceneTree* get_tree() const { return tree_; }
    void       set_tree(SceneTree* t) { tree_ = t; }

    // Name
    void        set_name(const std::string& n) { name_ = n; }
    std::string get_name() const { return name_; }
    StringID    get_name_id() const { return sid_string(name_); }

    // Path: /root/Main/Player
    std::string get_path() const;

    // Enable/disable
    void set_process(bool v)        { can_process_ = v; }
    void set_physics_process(bool v){ can_physics_ = v; }
    void set_visible(bool v)        { visible_ = v; }
    bool is_processing() const      { return can_process_; }
    bool is_visible() const         { return visible_; }

    // Metadata para el editor
    bool is_editor_only() const { return editor_only_; }
    void set_editor_only(bool v) { editor_only_ = v; }

    // Notifica el árbol de que se agregó.
    void propagate_notification(int what);

    // Buscar por ruta: /root/Main/Player ó ../Sibling
    Vox* get_node(const std::string& path) const;

protected:
    SceneTree*            tree_ = nullptr;
    Vox*                 parent_ = nullptr;
    std::vector<Vox*>    children_;
    std::string           name_;
    bool                  can_process_  = true;
    bool                  can_physics_  = true;
    bool                  visible_      = true;
    bool                  editor_only_  = false;
    bool                  inside_tree_  = false;

    friend class SceneTree;
};

// Notifications (what)
enum NodeNotification {
    NOTIFICATION_ENTER_TREE   = 10,
    NOTIFICATION_EXIT_TREE    = 11,
    NOTIFICATION_READY        = 12,
    NOTIFICATION_PROCESS      = 13,
    NOTIFICATION_PHYSICS      = 14,
    NOTIFICATION_DRAW         = 15,
    NOTIFICATION_PARENTED     = 16,
    NOTIFICATION_UNPARENTED   = 17
};

} // namespace arx
