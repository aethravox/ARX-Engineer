// ==============================================================================
// src/scene/node.cpp
// ==============================================================================
#include "vox.hpp"
#include "scene_tree.hpp"
#include "core/logging.hpp"
#include "core/string_db.hpp"

#include <algorithm>

namespace arx {

Vox::~Vox() {
    while (!children_.empty()) {
        Vox* c = children_.back();
        children_.pop_back();
        delete c;
    }
}

void Vox::add_child(Vox* child) {
    if (!child) return;
    if (child->parent_ == this) return;
    if (child->parent_) child->remove_from_parent();
    children_.push_back(child);
    child->parent_ = this;
    child->set_tree(tree_);
    child->propagate_notification(NOTIFICATION_PARENTED);
    if (tree_) tree_->add_node_to_tree(child);
}

void Vox::remove_child(Vox* child) {
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it == children_.end()) return;
    children_.erase(it);
    child->parent_ = nullptr;
    child->propagate_notification(NOTIFICATION_UNPARENTED);
    if (tree_) tree_->remove_node_from_tree(child);
}

void Vox::remove_from_parent() {
    if (parent_) parent_->remove_child(this);
}

Vox* Vox::get_child(const std::string& n) const {
    for (auto* c : children_) {
        if (c->name_ == n) return c;
    }
    return nullptr;
}

std::string Vox::get_path() const {
    if (!parent_) return "/" + name_;
    return parent_->get_path() + "/" + name_;
}

void Vox::propagate_notification(int what) {
    switch (what) {
        case NOTIFICATION_ENTER_TREE:
            inside_tree_ = true;
            _enter_tree();
            break;
        case NOTIFICATION_EXIT_TREE:
            inside_tree_ = false;
            _exit_tree();
            break;
        case NOTIFICATION_READY:    _ready();          break;
        case NOTIFICATION_PROCESS:  process(0.0f);    break;   // delta via tree
        case NOTIFICATION_DRAW:                      break;
        case NOTIFICATION_PARENTED:                   break;
        case NOTIFICATION_UNPARENTED:                 break;
    }
    for (auto* c : children_) c->propagate_notification(what);
}

Vox* Vox::get_node(const std::string& path) const {
    if (path.empty()) return nullptr;
    if (path[0] == '/') {
        // Absoluto: busca desde la raíz del árbol.
        Vox* r = tree_ ? tree_->get_root() : nullptr;
        if (!r) return nullptr;
        std::string p = path.substr(1);
        return r->get_node(p);
    }
    // Relativo: navegación por names con / y ..
    Vox* cur = const_cast<Vox*>(this);
    size_t i = 0;
    while (i < path.size()) {
        size_t j = path.find('/', i);
        std::string part = (j == std::string::npos) ? path.substr(i) : path.substr(i, j-i);
        if (part == "..") {
            cur = cur->parent_;
            if (!cur) return nullptr;
        } else if (!part.empty()) {
            cur = cur->get_child(part);
            if (!cur) return nullptr;
        }
        if (j == std::string::npos) break;
        i = j + 1;
    }
    return cur;
}

} // namespace arx
