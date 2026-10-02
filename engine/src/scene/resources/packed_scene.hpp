// ==============================================================================
// src/scene/resources/packed_scene.hpp / .cpp — Escena serializable.
// Una PackedScene es la versión serializada de un árbol de Nodes que se puede
// instanciar múltiples veces. Lee/escribe el formato .scene (texto).
// ==============================================================================
#pragma once

#include "scene/resources/resource.hpp"
#include "scene/vox.hpp"

#include <vector>
#include <string>
#include <unordered_map>

namespace arx {

// PackedScene: un árbol de NodeDefs serializado que se puede instanciar.
struct NodeDef {
    std::string name;
    std::string class_name;
    std::string parent_path;     // Vacío si es root
    std::string script_path;     // res://scripts/player.arx (opcional)
    std::unordered_map<std::string, Variant> properties;
    std::vector<NodeDef> children;
};

class PackedScene : public Resource {
public:
    ARX_CLASS(PackedScene, Resource);
public:

    bool load_from_file(const std::string& path);
    bool save_to_file(const std::string& path) const;

    // Instanciar como un Vox tree listo para añadir al SceneTree.
    Vox* instantiate() const;

    // Acceso.
    const NodeDef& get_root_def() const { return root_; }
    void set_root_def(NodeDef r) { root_ = std::move(r); }

    bool is_valid() const override { return !root_.class_name.empty(); }

private:
    NodeDef root_;

    Vox* instantiate_node(const NodeDef& def, Vox* parent) const;
    void  apply_properties(Vox* n, const NodeDef& def) const;
};

// SceneParser — parser del formato .scene (texto).
class SceneParser {
public:
    bool parse(const std::string& source, PackedScene& out);
    std::string serialize(const PackedScene& ps);

private:
    bool parse_node_def(const std::string& block, NodeDef& def,
                         const std::string& parent_path);
    Variant parse_property_value(const std::string& type_hint,
                                   const std::string& value_str);
};

// SceneLoader — loader automático para .scene (registrado en ResourceLoader).
void register_scene_loader();

} // namespace arx
