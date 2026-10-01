// ==============================================================================
// src/json/json.hpp — Parser y serializer JSON.
// ==============================================================================
#pragma once

#include "core/variant.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace arx::json {

// Value types.
enum class Type : uint8_t { Null, Bool, Int, Float, String, Array, Object };

class Value;
using Array  = std::vector<Value>;
using Object = std::unordered_map<std::string, Value>;

class Value {
public:
    Value() = default;
    Value(std::nullptr_t) : type_(Type::Null) {}
    Value(bool b)   : type_(Type::Bool),   bool_(b) {}
    Value(int i)    : type_(Type::Int),    int_(i) {}
    Value(int64_t i): type_(Type::Int),    int_(i) {}
    Value(double d) : type_(Type::Float),  float_(d) {}
    Value(const char* s) : type_(Type::String), string_(s) {}
    Value(std::string s) : type_(Type::String), string_(std::move(s)) {}
    Value(Array a)  : type_(Type::Array),  array_(std::move(a)) {}
    Value(Object o) : type_(Type::Object), object_(std::move(o)) {}

    Type type() const { return type_; }
    bool is_null()   const { return type_ == Type::Null; }
    bool is_bool()   const { return type_ == Type::Bool; }
    bool is_int()    const { return type_ == Type::Int; }
    bool is_float()  const { return type_ == Type::Float; }
    bool is_string() const { return type_ == Type::String; }
    bool is_array()  const { return type_ == Type::Array; }
    bool is_object() const { return type_ == Type::Object; }

    bool   as_bool()   const { return bool_; }
    int64_t as_int()   const { return int_; }
    double as_float()  const { return float_; }
    const std::string& as_string() const { return string_; }
    const Array&  as_array()  const { return array_; }
    const Object& as_object() const { return object_; }

    // Acceso a objetos.
    const Value& operator[](const std::string& key) const;
    Value&       operator[](const std::string& key);
    bool         has(const std::string& key) const;
    size_t       size() const;

private:
    Type type_ = Type::Null;
    bool   bool_   = false;
    int64_t int_   = 0;
    double float_  = 0.0;
    std::string string_;
    Array  array_;
    Object object_;
};

// API pública.
Value parse(const std::string& src, std::string* error = nullptr);
std::string serialize(const Value& v, bool pretty = false);

// Helpers Variant ↔ Value.
Value    from_variant(const Variant& v);
Variant  to_variant(const Value& v);

} // namespace arx::json
