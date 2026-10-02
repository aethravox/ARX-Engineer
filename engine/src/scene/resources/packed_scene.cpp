// ==============================================================================
#include "core/variant.hpp"
// src/scene/resources/packed_scene.cpp
//
// Formato .scene (texto):
//   [scene_main]
//   class = "Vox2D"
//   name  = "Main"
//
//   [child.Camera]
//   class = "VoxCamera2D"
//   name  = "Camera"
//   position = "0, 0"
//   zoom     = "1.0"
//
//   [child.Player.VoxSprite2D]
//   class = "VoxSprite2D"
//   texture = "res://sprites/player.png"
// ==============================================================================
#include "packed_scene.hpp"
#include "core/logging.hpp"
#include "core/object.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>

namespace arx {

// ===================== PackedScene ===========================================
bool PackedScene::load_from_file(const std::string& path) {
    std::ifstream f(path);
    if (!f) {
        ARX_LOG_ERROR("PackedScene: no se pudo abrir {}", path);
        return false;
    }
    std::stringstream ss;
    ss << f.rdbuf();
    SceneParser parser;
    return parser.parse(ss.str(), *this);
}

bool PackedScene::save_to_file(const std::string& path) const {
    std::ofstream f(path);
    if (!f) return false;
    SceneParser parser;
    f << parser.serialize(*this);
    return true;
}

Vox* PackedScene::instantiate() const {
    return instantiate_node(root_, nullptr);
}

Vox* PackedScene::instantiate_node(const NodeDef& def, Vox* parent) const {
    // Crear instancia vía ClassDB.
    StringID cls_id = sid(def.class_name.c_str());
    Object* obj = ClassDB::instance().instantiate(cls_id);
    Vox* node = dynamic_cast<Vox*>(obj);
    if (!node) {
        ARX_LOG_WARN("PackedScene: no se pudo instanciar '{}' (¿no registrado?)",
                     def.class_name);
        delete obj;
        return nullptr;
    }

    node->set_name(def.name);
    apply_properties(node, def);

    // Hijos.
    for (const auto& child_def : def.children) {
        Vox* child = instantiate_node(child_def, node);
        if (child) node->add_child(child);
    }

    (void)parent;  // parent se setea en add_child
    return node;
}

void PackedScene::apply_properties(Vox* n, const NodeDef& def) const {
    n->ensure_runtime_registry();
    for (const auto& [name, value] : def.properties) {
        StringID prop_id = sid(name.c_str());
        n->set_property(prop_id, value);
    }
}

// ===================== SceneParser ===========================================
bool SceneParser::parse(const std::string& source, PackedScene& out) {
    std::istringstream ss(source);
    std::string line;

    std::string current_block;
    std::string current_header;
    std::vector<std::string> block_lines;

    auto flush_block = [&]() {
        if (current_header.empty()) return;
        bool is_root = (current_header == "scene_main");
        std::string path = is_root ? "" : current_header;
        // Si empieza con "child.", remover prefijo.
        if (path.rfind("child.", 0) == 0) path = path.substr(6);

        NodeDef def;
        if (is_root) {
            // Root: path vacío.
            parse_node_def(current_header + "\n" + current_block, def, "");
            out.set_root_def(std::move(def));
        } else {
            // Es un hijo: lo añadimos al root_.
            NodeDef child_def;
            parse_node_def(current_header + "\n" + current_block, child_def, path);
            // Añadirlo al root_ (simplificación: solo soporta hijos directos;
            // para paths anidados como child.A.B se podría recorrer).
            auto& root = const_cast<NodeDef&>(out.get_root_def());
            root.children.push_back(std::move(child_def));
        }
        current_block.clear();
    };

    while (std::getline(ss, line)) {
        // Trim.
        std::string l = line;
        while (!l.empty() && std::isspace(l.front())) l.erase(0, 1);
        while (!l.empty() && std::isspace(l.back()))  l.pop_back();
        if (l.empty() || l[0] == '#') continue;

        if (l.front() == '[' && l.back() == ']') {
            // Nuevo bloque.
            flush_block();
            current_header = l.substr(1, l.size() - 2);
        } else {
            current_block += l + "\n";
        }
    }
    flush_block();
    return true;
}

bool SceneParser::parse_node_def(const std::string& block, NodeDef& def,
                                   const std::string& parent_path) {
    std::istringstream ss(block);
    std::string line;

    // Primera línea: [header]
    std::getline(ss, line);
    if (line.empty() || line.front() != '[') return false;
    std::string header = line.substr(1, line.size() - 2);

    if (header == "scene_main") {
        def.parent_path = "";
    } else {
        def.parent_path = parent_path;
        // El nombre puede estar al final del path.
        auto dot = header.find_last_of('.');
        def.name = (dot == std::string::npos) ? header : header.substr(dot + 1);
    }

    while (std::getline(ss, line)) {
        std::string l = line;
        while (!l.empty() && std::isspace(l.front())) l.erase(0, 1);
        if (l.empty()) continue;
        auto eq = l.find('=');
        if (eq == std::string::npos) continue;
        std::string key = l.substr(0, eq);
        std::string val = l.substr(eq + 1);
        while (!key.empty() && std::isspace(key.back())) key.pop_back();
        while (!val.empty() && std::isspace(val.front())) val.erase(0, 1);
        while (!val.empty() && std::isspace(val.back()))  val.pop_back();
        // Quitar comillas si las tiene.
        if (val.size() >= 2 && val.front() == '"' && val.back() == '"')
            val = val.substr(1, val.size() - 2);

        if (key == "class")        def.class_name = val;
        else if (key == "name")    def.name = val;
        else if (key == "script")  def.script_path = val;
        else if (key == "parent")  def.parent_path = val;
        else                       def.properties[key] = Variant(val);
    }
    return true;
}

Variant SceneParser::parse_property_value(const std::string& type_hint,
                                            const std::string& value_str) {
    // Simplificado: siempre devolvemos string. Una implementación completa
    // detectaría números, vectores, colores, etc. según type_hint.
    (void)type_hint;
    return Variant(value_str);
}

std::string SceneParser::serialize(const PackedScene& ps) {
    std::ostringstream out;
    const auto& root = ps.get_root_def();
    out << "[scene_main]\n";
    out << "class = \"" << root.class_name << "\"\n";
    out << "name = \"" << root.name << "\"\n";
    for (const auto& [k, v] : root.properties) {
        out << k << " = \"" << v.to_string() << "\"\n";
    }
    out << "\n";
    for (const auto& child : root.children) {
        out << "[child." << child.name << "]\n";
        out << "class = \"" << child.class_name << "\"\n";
        out << "name = \"" << child.name << "\"\n";
        for (const auto& [k, v] : child.properties) {
            out << k << " = \"" << v.to_string() << "\"\n";
        }
        out << "\n";
    }
    return out.str();
}

// ===================== SceneLoader registration =============================
void register_scene_loader() {
    ResourceLoader::instance().register_loader("scene",
        [](const std::string& path) -> ResourcePtr {
            auto ps = std::make_shared<PackedScene>();
            if (ps->load_from_file(path)) return ps;
            return nullptr;
        });
}

} // namespace arx
