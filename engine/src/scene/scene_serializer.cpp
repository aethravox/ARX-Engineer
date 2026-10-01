// ==============================================================================
#include "core/variant.hpp"
// src/scene/scene_serializer.cpp
// ==============================================================================
#include "scene_serializer.hpp"
#include "core/object.hpp"
#include "core/logging.hpp"
#include "core/string_db.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>

namespace arx {

// ===================== PackedScene ===========================================
Vox* PackedScene::instantiate() const {
    if (nodes_.empty()) return nullptr;

    std::unordered_map<std::string, Vox*> created;

    Vox* root = nullptr;
    for (const auto& nd : nodes_) {
        auto* info = ClassDB::instance().get_class(sid(nd.class_name.c_str()));
        if (!info || !info->factory) {
            ARX_LOG_WARN("PackedScene::instantiate: clase '{}' no registrada",
                         nd.class_name);
            continue;
        }
        Vox* n = dynamic_cast<Vox*>(info->factory());
        if (!n) {
            ARX_LOG_WARN("PackedScene::instantiate: '{}' no es Vox", nd.class_name);
            continue;
        }
        n->set_name(nd.name);
        for (const auto& [k, v] : nd.properties) {
            // Parse value depending on type (we don't know types here).
            // Simplificación: intentar como string o como float.
            Variant var;
            if (v.find(',') != std::string::npos) {
                // Could be vector2/3/color. Try vector2.
                std::stringstream ss(v);
                float x, y;
                char c;
                if (ss >> x >> c >> y) {
                    var = Variant(Vector2(x, y));
                }
            } else {
                // Try as float.
                try {
                    float f = std::stof(v);
                    var = Variant(f);
                } catch (...) {
                    var = Variant(v);
                }
            }
            n->set_property(sid(k.c_str()), var);
        }
        created[nd.name] = n;
        if (nd.parent_path.empty()) {
            root = n;
        } else {
            auto it = created.find(nd.parent_path);
            if (it != created.end()) it->second->add_child(n);
        }
    }
    return root;
}

bool PackedScene::save_to_file(const std::string& path) const {
    std::ofstream out(path);
    if (!out) return false;
    out << "# ARX Engine scene file\n";
    out << "[scene_main]\n";
    for (size_t i = 0; i < nodes_.size(); ++i) {
        const auto& nd = nodes_[i];
        out << "\n[child." << nd.name << "]\n";
        out << "class = \"" << nd.class_name << "\"\n";
        out << "name  = \"" << nd.name << "\"\n";
        if (!nd.parent_path.empty())
            out << "parent = \"" << nd.parent_path << "\"\n";
        for (const auto& [k, v] : nd.properties) {
            out << k << " = \"" << v << "\"\n";
        }
    }
    return true;
}

std::shared_ptr<PackedScene> PackedScene::load_from_file(const std::string& path) {
    return SceneSerializer::load(path);
}

std::shared_ptr<PackedScene> PackedScene::pack(Vox* root) {
    auto ps = std::make_shared<PackedScene>();
    if (!root) return ps;

    std::function<void(Vox*, const std::string&)> walk = [&](Vox* n,
                                                                 const std::string& parent_path) {
        PackedScene::NodeData nd;
        nd.class_name = std::string(n->get_class_name());
        nd.name       = n->get_name();
        nd.parent_path = parent_path;
        // TODO: extraer properties reales desde el ClassDB.
        ps->add_node(nd);
        for (auto* c : n->get_children()) {
            walk(c, nd.name);
        }
    };
    walk(root, "");
    return ps;
}

// ===================== SceneSerializer =======================================
std::shared_ptr<PackedScene> SceneSerializer::load(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        ARX_LOG_ERROR("SceneSerializer: no se pudo abrir {}", path);
        return nullptr;
    }

    auto ps = std::make_shared<PackedScene>();
    std::string line;
    PackedScene::NodeData* current = nullptr;

    while (std::getline(in, line)) {
        // Trim.
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (line.empty() || line[0] == '#') continue;
        if (line[0] == '[' && line.back() == ']') {
            // Section header.
            if (line.find("[child.") == 0) {
                ps->add_node({});
                current = const_cast<PackedScene::NodeData*>(&ps->get_nodes().back());
            } else {
                current = nullptr;  // [scene_main] etc.
            }
            continue;
        }
        // key = "value"
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = line.substr(0, eq);
        std::string val = line.substr(eq + 1);
        // Trim both.
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        val.erase(0, val.find_first_not_of(" \t"));
        val.erase(val.find_last_not_of(" \t") + 1);
        // Strip quotes.
        if (val.size() >= 2 && val.front() == '"' && val.back() == '"')
            val = val.substr(1, val.size() - 2);

        if (!current) continue;
        if (key == "class") current->class_name = val;
        else if (key == "name")  current->name = val;
        else if (key == "parent") current->parent_path = val;
        else current->properties[key] = val;
    }
    return ps;
}

bool SceneSerializer::save(Vox* root, const std::string& path) {
    auto ps = PackedScene::pack(root);
    return ps->save_to_file(path);
}

Vox* SceneSerializer::load_and_instantiate(const std::string& path) {
    auto ps = load(path);
    if (!ps) return nullptr;
    return ps->instantiate();
}

} // namespace arx
