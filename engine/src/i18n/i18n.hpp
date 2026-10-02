// ==============================================================================
// src/i18n/i18n.hpp — Internacionalización.
// ==============================================================================
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace arx {

// Translation — un diccionario key → texto traducido para un locale.
class Translation {
public:
    void set_locale(const std::string& l) { locale_ = l; }
    const std::string& get_locale() const { return locale_; }

    void add_message(const std::string& key, const std::string& text) {
        messages_[key] = text;
    }
    std::string get_message(const std::string& key) const {
        auto it = messages_.find(key);
        return it != messages_.end() ? it->second : key;
    }
    bool has_message(const std::string& key) const {
        return messages_.find(key) != messages_.end();
    }
    size_t message_count() const { return messages_.size(); }

    // Carga CSV: key,en,es,fr,...
    bool load_csv(const std::string& path, const std::string& column);

private:
    std::string locale_;
    std::unordered_map<std::string, std::string> messages_;
};

// I18nServer — servidor de traducciones.
class I18nServer {
public:
    static I18nServer& instance();

    void set_locale(const std::string& l);
    const std::string& get_locale() const { return locale_; }

    void add_translation(const Translation& t);
    void clear_translations();

    // Tr() — devuelve la traducción al locale actual.
    std::string tr(const std::string& key) const;
    std::string tr(const std::string& key, const std::string& fallback) const;

    std::vector<std::string> get_available_locales() const;

private:
    I18nServer() = default;
    std::string locale_ = "en";
    std::unordered_map<std::string, Translation> translations_;
};

// Helper de conveniencia.
inline std::string tr(const std::string& key) {
    return I18nServer::instance().tr(key);
}

} // namespace arx
