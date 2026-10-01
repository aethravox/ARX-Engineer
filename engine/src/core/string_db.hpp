// ==============================================================================
// src/core/string_db.hpp — strings internados para uso del motor.
// ==============================================================================
#pragma once

#include "core/types.hpp"

// En algunas versiones de libstdc++ con GCC 11, <mutex> tira errores porque
// <ctime> no incluye todo lo necesario. Lo incluimos primero explícitamente.
#include <ctime>
#include <string>
#include <string_view>
#include <unordered_map>
#include <mutex>

namespace arx {

// Registro global de strings internados. Sirve para que dos StringID generados
// desde el mismo literal de chars siempre tengan el mismo hash Y puedan ser
// convertidos de vuelta a std::string_view en cualquier momento.
class StringDB {
public:
    static StringDB& instance() {
        static StringDB inst;
        return inst;
    }

    StringID intern(std::string_view s) {
        StringID id = sid(s.data());
        std::lock_guard<std::mutex> lock(mtx_);
        if (storage_.find(id) == storage_.end()) {
            storage_[id] = std::string(s);
        }
        return id;
    }

    std::string_view resolve(StringID id) const {
        auto it = storage_.find(id);
        if (it == storage_.end()) return {};
        return it->second;
    }

private:
    StringDB() = default;
    mutable std::mutex mtx_;
    mutable std::unordered_map<StringID, std::string> storage_;
};

// Helper
inline StringID sid_string(std::string_view s) {
    return StringDB::instance().intern(s);
}

} // namespace arx
