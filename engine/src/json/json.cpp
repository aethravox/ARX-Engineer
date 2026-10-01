// ==============================================================================
#include "core/variant.hpp"
// src/json/json.cpp — Parser + serializer JSON.
// ==============================================================================
#include "json.hpp"

#include <sstream>
#include <cctype>
#include <cmath>
#include <cstring>
#include <iomanip>

namespace arx::json {

// ===================== Value accessors ======================================
const Value& Value::operator[](const std::string& key) const {
    static Value null_val;
    auto it = object_.find(key);
    return (it != object_.end()) ? it->second : null_val;
}

Value& Value::operator[](const std::string& key) {
    type_ = Type::Object;
    return object_[key];
}

bool Value::has(const std::string& key) const {
    return object_.find(key) != object_.end();
}

size_t Value::size() const {
    if (type_ == Type::Array)  return array_.size();
    if (type_ == Type::Object) return object_.size();
    return 0;
}

// ===================== Parser ================================================
namespace {

struct Parser {
    const char* p;
    const char* end;
    std::string error;

    char peek() { return (p < end) ? *p : '\0'; }
    char next() { return (p < end) ? *p++ : '\0'; }
    void skip_ws() {
        while (p < end && (std::isspace((unsigned char)*p))) ++p;
    }

    bool match(char c) {
        skip_ws();
        if (peek() == c) { ++p; return true; }
        return false;
    }

    bool expect(char c, const char* what) {
        if (!match(c)) {
            error = std::string("expected ") + what;
            return false;
        }
        return true;
    }

    Value parse_value() {
        skip_ws();
        char c = peek();
        if (c == '{') return parse_object();
        if (c == '[') return parse_array();
        if (c == '"') return parse_string();
        if (c == '-' || std::isdigit((unsigned char)c)) return parse_number();
        if (std::strncmp(p, "true", 4) == 0)  { p += 4; return Value(true); }
        if (std::strncmp(p, "false", 5) == 0) { p += 5; return Value(false); }
        if (std::strncmp(p, "null", 4) == 0)  { p += 4; return Value(nullptr); }
        error = "unexpected token";
        return nullptr;
    }

    Value parse_object() {
        Object obj;
        ++p; // consume {
        skip_ws();
        if (peek() == '}') { ++p; return Value(std::move(obj)); }
        while (true) {
            skip_ws();
            if (peek() != '"') { error = "expected string key"; return nullptr; }
            Value key = parse_string();
            if (!error.empty()) return nullptr;
            skip_ws();
            if (!expect(':', "':'")) return nullptr;
            Value v = parse_value();
            if (!error.empty()) return nullptr;
            obj[key.as_string()] = std::move(v);
            skip_ws();
            if (peek() == ',') { ++p; continue; }
            if (peek() == '}') { ++p; break; }
            error = "expected ',' or '}'";
            return nullptr;
        }
        return Value(std::move(obj));
    }

    Value parse_array() {
        Array arr;
        ++p; // consume [
        skip_ws();
        if (peek() == ']') { ++p; return Value(std::move(arr)); }
        while (true) {
            Value v = parse_value();
            if (!error.empty()) return nullptr;
            arr.push_back(std::move(v));
            skip_ws();
            if (peek() == ',') { ++p; continue; }
            if (peek() == ']') { ++p; break; }
            error = "expected ',' or ']'";
            return nullptr;
        }
        return Value(std::move(arr));
    }

    Value parse_string() {
        ++p; // consume "
        std::string s;
        while (p < end && *p != '"') {
            if (*p == '\\') {
                ++p;
                if (p >= end) break;
                char e = *p++;
                switch (e) {
                    case '"': s += '"'; break;
                    case '\\': s += '\\'; break;
                    case '/': s += '/'; break;
                    case 'n': s += '\n'; break;
                    case 't': s += '\t'; break;
                    case 'r': s += '\r'; break;
                    case 'b': s += '\b'; break;
                    case 'f': s += '\f'; break;
                    case 'u': {
                        // Simplificado: incluir los 4 hex chars.
                        s += "\\u";
                        for (int i = 0; i < 4 && p < end; ++i) s += *p++;
                        break;
                    }
                    default: s += e;
                }
            } else {
                s += *p++;
            }
        }
        if (p < end) ++p; // consume closing "
        return Value(std::move(s));
    }

    Value parse_number() {
        std::string num;
        bool is_float = false;
        while (p < end) {
            char c = *p;
            if (std::isdigit((unsigned char)c) || c == '-' || c == '+') {
                num += c; ++p;
            } else if (c == '.' || c == 'e' || c == 'E') {
                is_float = true; num += c; ++p;
            } else break;
        }
        if (is_float) return Value(std::stod(num));
        return Value((int64_t)std::stoll(num));
    }
};

void write_string_escaped(std::ostream& os, const std::string& s) {
    os << '"';
    for (char c : s) {
        switch (c) {
            case '"':  os << "\\\""; break;
            case '\\': os << "\\\\"; break;
            case '\n': os << "\\n"; break;
            case '\t': os << "\\t"; break;
            case '\r': os << "\\r"; break;
            case '\b': os << "\\b"; break;
            case '\f': os << "\\f"; break;
            default:
                if ((unsigned char)c < 0x20) {
                    os << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)(unsigned char)c;
                    os << std::dec;
                } else {
                    os << c;
                }
        }
    }
    os << '"';
}

void write_value(std::ostream& os, const Value& v, bool pretty, int indent) {
    auto pad = [&](int n) { if (pretty) for (int i=0;i<n;++i) os << "  "; };
    switch (v.type()) {
        case Type::Null:   os << "null"; break;
        case Type::Bool:   os << (v.as_bool() ? "true" : "false"); break;
        case Type::Int:    os << v.as_int(); break;
        case Type::Float:  os << v.as_float(); break;
        case Type::String: write_string_escaped(os, v.as_string()); break;
        case Type::Array: {
            os << '[';
            if (pretty && !v.as_array().empty()) os << '\n';
            bool first = true;
            for (const auto& item : v.as_array()) {
                if (!first) { os << (pretty ? ",\n" : ","); }
                pad(indent + 1);
                write_value(os, item, pretty, indent + 1);
                first = false;
            }
            if (pretty && !v.as_array().empty()) { os << '\n'; pad(indent); }
            os << ']';
            break;
        }
        case Type::Object: {
            os << '{';
            if (pretty && !v.as_object().empty()) os << '\n';
            bool first = true;
            for (const auto& [k, val] : v.as_object()) {
                if (!first) { os << (pretty ? ",\n" : ","); }
                pad(indent + 1);
                write_string_escaped(os, k);
                os << (pretty ? ": " : ":");
                write_value(os, val, pretty, indent + 1);
                first = false;
            }
            if (pretty && !v.as_object().empty()) { os << '\n'; pad(indent); }
            os << '}';
            break;
        }
    }
}

} // anonymous

// ===================== API pública ===========================================
Value parse(const std::string& src, std::string* error) {
    Parser p{ src.data(), src.data() + src.size(), "" };
    Value v = p.parse_value();
    if (!p.error.empty()) {
        if (error) *error = p.error;
        return nullptr;
    }
    return v;
}

std::string serialize(const Value& v, bool pretty) {
    std::ostringstream os;
    write_value(os, v, pretty, 0);
    return os.str();
}

// ===================== Variant ↔ JSON Value ==================================
Value from_variant(const Variant& v) {
    switch (v.get_type()) {
        case Variant::Type::Nil:    return nullptr;
        case Variant::Type::Bool:   return v.to_bool();
        case Variant::Type::Int:    return v.to_int();
        case Variant::Type::Float:  return v.to_float();
        case Variant::Type::String: return v.to_string();
        default: return v.to_string();   // Fallback
    }
}

Variant to_variant(const Value& v) {
    switch (v.type()) {
        case Type::Null:   return Variant();
        case Type::Bool:   return Variant(v.as_bool());
        case Type::Int:    return Variant(v.as_int());
        case Type::Float:  return Variant(v.as_float());
        case Type::String: return Variant(v.as_string());
        default:           return Variant();
    }
}

} // namespace arx::json
