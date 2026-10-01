// ==============================================================================
// src/core/variant.hpp — Variant: contenedor de tipos dinámicos.
// Se usa para el editor (inspector) y para ARXScript (tipado dinámico).
//
// Nota: para evitar recursión (Variant no puede contener std::vector<Variant>
// directamente porque Variant no estaría completo), usamos shared_ptr para
// Array y Dictionary. Es lo mismo que hacen Godot y muchos otros motores.
// ==============================================================================
#pragma once

#include "core/types.hpp"
#include "core/object.hpp"

#include <string>
#include <variant>
#include <vector>
#include <cstdint>
#include <memory>
#include <unordered_map>

namespace arx {

class Object;

// Array y Dictionary como wrappers con shared_ptr (rompe recursión).
class Variant;
using Array     = std::vector<Variant>;
using Dictionary = std::unordered_map<std::string, Variant>;

class Variant {
public:
    enum class Type : uint8_t {
        Nil, Bool, Int, Float, String,
        Vector2, Vector3, Vector4, Color, Rect2,
        Object, Array, Dictionary
    };

    // Storage: usar shared_ptr para Array/Dictionary (Variant incompleto).
    using Storage = std::variant<
        std::monostate,
        bool,
        int64_t,
        double,
        std::string,
        Vector2,
        Vector3,
        Vector4,
        Color,
        Rect2,
        Object*,
        std::shared_ptr<Array>,
        std::shared_ptr<Dictionary>
    >;

    Variant() = default;
    Variant(std::monostate) {}
    Variant(bool v)              : storage_(v) {}
    Variant(int32_t v)           : storage_(int64_t(v)) {}
    Variant(int64_t v)           : storage_(v) {}
    Variant(long long v)         : storage_(int64_t(v)) {}
    Variant(unsigned int v)      : storage_(int64_t(v)) {}
    Variant(unsigned long v)     : storage_(int64_t(v)) {}
    Variant(float v)             : storage_(double(v)) {}
    Variant(double v)            : storage_(v) {}
    Variant(const char* s)       : storage_(std::string(s)) {}
    Variant(std::string s)       : storage_(std::move(s)) {}
    Variant(Vector2 v)           : storage_(v) {}
    Variant(Vector3 v)           : storage_(v) {}
    Variant(Vector4 v)           : storage_(v) {}
    Variant(Color v)             : storage_(v) {}
    Variant(Rect2 v)             : storage_(v) {}
    Variant(Object* o)           : storage_(o) {}

    // Array y Dictionary: convertir de vector/map a shared_ptr.
    Variant(const Array& a)       : storage_(std::make_shared<Array>(a)) {}
    Variant(Array&& a)            : storage_(std::make_shared<Array>(std::move(a))) {}
    Variant(const Dictionary& d)  : storage_(std::make_shared<Dictionary>(d)) {}
    Variant(Dictionary&& d)       : storage_(std::make_shared<Dictionary>(std::move(d))) {}

    Type get_type() const {
        struct Visitor {
            Type operator()(std::monostate) const { return Type::Nil; }
            Type operator()(bool)               const { return Type::Bool; }
            Type operator()(int64_t)            const { return Type::Int; }
            Type operator()(double)             const { return Type::Float; }
            Type operator()(const std::string&) const { return Type::String; }
            Type operator()(Vector2)            const { return Type::Vector2; }
            Type operator()(Vector3)            const { return Type::Vector3; }
            Type operator()(Vector4)            const { return Type::Vector4; }
            Type operator()(Color)              const { return Type::Color; }
            Type operator()(Rect2)              const { return Type::Rect2; }
            Type operator()(Object*)            const { return Type::Object; }
            Type operator()(const std::shared_ptr<Array>&)      const { return Type::Array; }
            Type operator()(const std::shared_ptr<Dictionary>&) const { return Type::Dictionary; }
        };
        return std::visit(Visitor{}, storage_);
    }

    bool is_nil() const { return get_type() == Type::Nil; }

    // Conversión tipada con fallback.
    bool     to_bool()   const {
        if (auto v = std::get_if<bool>(&storage_))     return *v;
        if (auto v = std::get_if<int64_t>(&storage_))  return *v != 0;
        if (auto v = std::get_if<double>(&storage_))   return *v != 0.0;
        return false;
    }
    int64_t  to_int()    const {
        if (auto v = std::get_if<int64_t>(&storage_))  return *v;
        if (auto v = std::get_if<double>(&storage_))   return (int64_t)*v;
        if (auto v = std::get_if<bool>(&storage_))     return *v ? 1 : 0;
        return 0;
    }
    double   to_float()  const {
        if (auto v = std::get_if<double>(&storage_))   return *v;
        if (auto v = std::get_if<int64_t>(&storage_))  return (double)*v;
        return 0.0;
    }
    std::string to_string() const {
        if (auto v = std::get_if<std::string>(&storage_)) return *v;
        return std::visit([](auto&& arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, bool>)               return arg ? "true" : "false";
            else if constexpr (std::is_same_v<T, int64_t>)       return std::to_string(arg);
            else if constexpr (std::is_same_v<T, double>)        return std::to_string(arg);
            else if constexpr (std::is_same_v<T, std::monostate>)return "null";
            else return "<non-string>";
        }, storage_);
    }
    Vector2 to_vector2() const {
        if (auto v = std::get_if<Vector2>(&storage_)) return *v;
        if (auto v = std::get_if<Vector3>(&storage_)) return Vector2(v->x, v->y);
        return {};
    }
    Vector3 to_vector3() const {
        if (auto v = std::get_if<Vector3>(&storage_)) return *v;
        if (auto v = std::get_if<Vector2>(&storage_)) return Vector3(v->x, v->y, 0);
        return {};
    }
    Color   to_color() const {
        if (auto v = std::get_if<Color>(&storage_)) return *v;
        return Color::white;
    }
    Rect2   to_rect2() const {
        if (auto v = std::get_if<Rect2>(&storage_)) return *v;
        return {};
    }
    Object* to_object() const {
        if (auto v = std::get_if<Object*>(&storage_)) return *v;
        return nullptr;
    }
    Array to_array() const {
        if (auto v = std::get_if<std::shared_ptr<Array>>(&storage_))
            return (v && *v) ? **v : Array{};
        return {};
    }
    Dictionary to_dictionary() const {
        if (auto v = std::get_if<std::shared_ptr<Dictionary>>(&storage_))
            return (v && *v) ? **v : Dictionary{};
        return {};
    }

    // Acceso a array/dict mutables (crea el contenedor si no existe).
    Array& array_ref() {
        if (auto v = std::get_if<std::shared_ptr<Array>>(&storage_)) {
            if (!*v) *v = std::make_shared<Array>();
            return **v;
        }
        auto p = std::make_shared<Array>();
        storage_ = p;
        return *p;
    }
    Dictionary& dict_ref() {
        if (auto v = std::get_if<std::shared_ptr<Dictionary>>(&storage_)) {
            if (!*v) *v = std::make_shared<Dictionary>();
            return **v;
        }
        auto p = std::make_shared<Dictionary>();
        storage_ = p;
        return *p;
    }

    const Storage& raw() const { return storage_; }

    bool operator==(const Variant& o) const { return storage_ == o.storage_; }
    bool operator!=(const Variant& o) const { return !(*this == o); }

private:
    Storage storage_;
};

inline std::string variant_to_string(const Variant& v) {
    return v.to_string();
}

} // namespace arx
