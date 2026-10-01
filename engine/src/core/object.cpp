// ==============================================================================
#include "core/variant.hpp"
// src/core/object.cpp
// ==============================================================================
#include "core/object.hpp"
#include "core/logging.hpp"

namespace arx {

// =============================== Signal ======================================
void Signal::connect(Object* receiver, Slot slot) {
    slots_.push_back({receiver, std::move(slot)});
}

void Signal::disconnect(Object* receiver) {
    slots_.erase(
        std::remove_if(slots_.begin(), slots_.end(),
            [receiver](const SlotEntry& e) { return e.receiver == receiver; }),
        slots_.end());
}

void Signal::emit(Object* sender, std::vector<Variant> args) {
    for (auto& entry : slots_) {
        entry.slot(sender, args);
    }
}

// =============================== ClassDB =====================================
ClassDB& ClassDB::instance() {
    static ClassDB db;
    return db;
}

ClassDB::ClassInfo* ClassDB::register_class(StringID id, StringID parent,
                                            std::function<Object*()> factory) {
    ClassInfo info;
    info.class_id = id;
    info.parent_id = parent;
    info.factory = std::move(factory);
    auto [it, _] = classes_.emplace(id, std::move(info));
    return &it->second;
}

ClassDB::ClassInfo* ClassDB::get_class(StringID id) {
    auto it = classes_.find(id);
    if (it == classes_.end()) return nullptr;
    return &it->second;
}

Object* ClassDB::instantiate(StringID id) {
    auto* info = get_class(id);
    if (!info || !info->factory) {
        ARX_LOG_ERROR("ClassDB: no se pudo instanciar '{}'", static_cast<uint64_t>(id.hash));
        return nullptr;
    }
    return info->factory();
}

// =============================== Object ======================================
const std::vector<Property>& Object::get_properties() const {
    return properties_;
}

Variant Object::get_property(StringID name) const {
    for (const auto& p : properties_) {
        if (p.name == name) return p.get(this);
    }
    return {};
}

void Object::set_property(StringID name, const Variant& v) {
    for (auto& p : properties_) {
        if (p.name == name) { p.set(this, v); return; }
    }
    ARX_LOG_WARN("Object::set_property: propiedad '{}' no encontrada",
                 static_cast<uint64_t>(name.hash));
}

Variant Object::call_method(StringID name, const std::vector<Variant>& args) {
    auto it = methods_.find(name);
    if (it == methods_.end()) {
        ARX_LOG_WARN("Object::call_method: método '{}' no encontrado",
                     static_cast<uint64_t>(name.hash));
        return {};
    }
    return it->second(this, args);
}

void Object::connect(StringID signal_name, Object* receiver, Signal::Slot slot) {
    signals_[signal_name].connect(receiver, std::move(slot));
}

void Object::emit_signal(StringID signal_name, std::vector<Variant> args) {
    auto it = signals_.find(signal_name);
    if (it != signals_.end()) {
        it->second.emit(this, std::move(args));
    }
}

void Object::ensure_runtime_registry() {
    auto* info = ClassDB::instance().get_class(get_class_id());
    if (!info) return;
    if (properties_.empty() && !info->properties.empty()) {
        properties_ = info->properties;
    }
    if (methods_.empty() && !info->methods.empty()) {
        methods_ = info->methods;
    }
}

} // namespace arx
