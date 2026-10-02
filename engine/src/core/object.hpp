// ==============================================================================
// src/core/object.hpp — Base de todos los objetos del motor.
// Soporta signals, properties, metodos expuestos a scripting.
// ==============================================================================
#pragma once

#include "core/types.hpp"
#include "core/string_db.hpp"

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>
#include <any>
#include <atomic>

namespace arx {

class Object;
class Variant;

// Firma de un método C++ invocable desde ARXScript o el editor.
using MethodCallable = std::function<Variant(Object*, const std::vector<Variant>&)>;

// Property: getter + setter de un atributo exponible.
struct Property {
    StringID           name;
    std::string        type_name;
    std::function<Variant(const Object*)> get;
    std::function<void(Object*, const Variant&)> set;
};

// Signal: callbacks conectables.
class Signal {
public:
    friend class PackedScene;
    using Slot = std::function<void(Object*, const std::vector<Variant>&)>;

    void connect(Object* receiver, Slot slot);
    void disconnect(Object* receiver);
    void emit(Object* sender, std::vector<Variant> args = {});

private:
    struct SlotEntry {
        Object* receiver;
        Slot    slot;
    };
    std::vector<SlotEntry> slots_;
};

// ==============================================================================
// ClassDB — registro estático de clases (estilo Godot ClassDB).
// Permite instanciar clases por nombre, listar métodos/properties.
// ==============================================================================
class ClassDB {
public:
    struct ClassInfo {
        StringID                          class_id;
        std::string                       class_name;
        StringID                          parent_id;
        std::function<Object*()>          factory;
        std::vector<Property>             properties;
        std::unordered_map<StringID, MethodCallable> methods;
        std::vector<std::pair<StringID, std::string>> signals; // name, arg_desc
    };

    static ClassDB& instance();

    ClassInfo* register_class(StringID id, StringID parent, std::function<Object*()> factory);
    ClassInfo* get_class(StringID id);
    Object*    instantiate(StringID id);

    template<typename T>
    static StringID register_class() {
        static_assert(std::is_base_of_v<Object, T>,
            "T debe heredar de arx::Object");
        auto& db = instance();
        auto id  = T::get_class_id_static();
        ClassInfo* info = db.register_class(id, T::get_parent_class_id_static(),
                                            []() -> Object* { return new T(); });
        T::bind_methods(info);
        T::bind_properties(info);
        T::bind_signals(info);
        return id;
    }

private:
    ClassDB() = default;
    std::unordered_map<StringID, ClassInfo> classes_;
};

// ==============================================================================
// Object — base de todo.
// ==============================================================================
class Object {
public:
    void ensure_runtime_registry();
    friend class PackedScene;
    Object() = default;
    virtual ~Object() = default;

    // Identidad
    virtual StringID get_class_id() const = 0;
    virtual std::string_view get_class_name() const = 0;
    static  StringID get_parent_class_id_static() { return StringID{}; }
    static  StringID get_class_id_static() { return sid("Object"); }

    // Metodos que la subclase registra en ClassDB.
    static void bind_methods(ClassDB::ClassInfo*) {}
    static void bind_properties(ClassDB::ClassInfo*) {}
    static void bind_signals(ClassDB::ClassInfo*) {}

    // Properties runtime
    const std::vector<Property>& get_properties() const;
    Variant get_property(StringID name) const;
    void    set_property(StringID name, const Variant& v);

    // Métodos invocables
    Variant call_method(StringID name, const std::vector<Variant>& args);

    // Signals runtime
    void connect(StringID signal_name, Object* receiver, Signal::Slot slot);
    void emit_signal(StringID signal_name, std::vector<Variant> args = {});

    // Metadatos (lo usa el editor)
    void set_meta(const std::string& key, std::any value) { meta_[key] = std::move(value); }
    template<typename T>
    T get_meta(const std::string& key, T fallback = T{}) const {
        auto it = meta_.find(key);
        if (it == meta_.end()) return fallback;
        try { return std::any_cast<T>(it->second); }
        catch (...) { return fallback; }
    }

    // Identidad numérica del objeto (para hashes/debug).
    uint64_t get_instance_id() const { return instance_id_; }

protected:
    // Registro runtime (copia los del ClassDB para permitir override por instancia).

private:
    void ensure_id() const {
        if (instance_id_ == 0) {
            static std::atomic<uint64_t> counter{1};
            instance_id_ = counter++;
        }
    }

    mutable uint64_t instance_id_ = 0;
    std::unordered_map<std::string, std::any> meta_;
    std::unordered_map<StringID, Signal>      signals_;
    std::vector<Property>                     properties_;
    std::unordered_map<StringID, MethodCallable> methods_;
};

// Macro para declarar una clase registrable en ClassDB.
#define ARX_CLASS(Class, Parent)                                                \
public:                                                                         \
    StringID get_class_id() const override          { return get_class_id_static(); } \
    std::string_view get_class_name() const override { return #Class; }         \
    static  StringID get_class_id_static()          { return ::arx::sid(#Class); } \
    static  StringID get_parent_class_id_static()   { return Parent::get_class_id_static(); } \
public:

} // namespace arx
