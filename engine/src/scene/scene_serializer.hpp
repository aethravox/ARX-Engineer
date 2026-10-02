// ==============================================================================
// src/scene/scene_serializer.hpp — Serializar/deserializar escenas a .scene.
// Formato de texto simple, estilo INI/Conf (ver project_templates/.../*.scene).
// ==============================================================================
#pragma once

#include "scene/vox.hpp"

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

namespace arx {

// PackedScene: escena serializada, instanciable múltiples veces.
class PackedScene : public Object {
public:
    ARX_CLASS(PackedScene, Object);
public:

    struct NodeData {
        std::string class_name;
        std::string name;
        std::string parent_path;     // Path al padre dentro del packed scene
        std::unordered_map<std::string, std::string> properties;
    };

    void add_node(const NodeData& d) { nodes_.push_back(d); }
    const std::vector<NodeData>& get_nodes() const { return nodes_; }

    // Instanciar el árbol.
    Vox* instantiate() const;

    // Serialize/deserialize.
    bool save_to_file(const std::string& path) const;
    static std::shared_ptr<PackedScene> load_from_file(const std::string& path);

    // Pack from existing Vox tree.
    static std::shared_ptr<PackedScene> pack(Vox* root);

private:
    std::vector<NodeData> nodes_;
};

class SceneSerializer {
public:
    // Carga una escena desde un .scene file.
    static std::shared_ptr<PackedScene> load(const std::string& path);

    // Guarda una escena a un .scene file.
    static bool save(Vox* root, const std::string& path);

    // Carga un .scene y devuelve el Vox* raíz instanciado.
    static Vox* load_and_instantiate(const std::string& path);
};

} // namespace arx
