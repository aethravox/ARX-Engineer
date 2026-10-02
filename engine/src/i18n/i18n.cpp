// ==============================================================================
// src/i18n/i18n.cpp
// ==============================================================================
#include "i18n.hpp"
#include "core/logging.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>

namespace arx {

I18nServer& I18nServer::instance() {
    static I18nServer s;
    return s;
}

void I18nServer::set_locale(const std::string& l) {
    locale_ = l;
    ARX_LOG_INFO("I18n: locale = {}", l);
}

void I18nServer::add_translation(const Translation& t) {
    translations_[t.get_locale()] = t;
}

void I18nServer::clear_translations() {
    translations_.clear();
}

std::string I18nServer::tr(const std::string& key) const {
    auto it = translations_.find(locale_);
    if (it != translations_.end()) return it->second.get_message(key);
    return key;
}

std::string I18nServer::tr(const std::string& key, const std::string& fallback) const {
    auto it = translations_.find(locale_);
    if (it != translations_.end() && it->second.has_message(key))
        return it->second.get_message(key);
    return fallback;
}

std::vector<std::string> I18nServer::get_available_locales() const {
    std::vector<std::string> out;
    out.reserve(translations_.size());
    for (const auto& [l, _] : translations_) out.push_back(l);
    return out;
}

bool Translation::load_csv(const std::string& path, const std::string& column) {
    std::ifstream f(path);
    if (!f) return false;
    std::string line;
    std::getline(f, line);  // header
    // Parsear header para encontrar el índice de column.
    std::stringstream hs(line);
    std::string cell;
    int target_col = -1, idx = 0;
    while (std::getline(hs, cell, ',')) {
        if (cell == column) { target_col = idx; break; }
        ++idx;
    }
    if (target_col < 0) return false;

    while (std::getline(f, line)) {
        std::stringstream ss(line);
        std::string key, val;
        // col 0 = key.
        if (!std::getline(ss, key, ',')) continue;
        // Saltar al target_col.
        for (int i = 1; i < target_col; ++i) std::getline(ss, cell, ',');
        if (!std::getline(ss, val, ',')) continue;
        if (!key.empty()) add_message(key, val);
    }
    return true;
}

} // namespace arx
