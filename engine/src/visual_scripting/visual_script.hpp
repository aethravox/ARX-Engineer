// ==============================================================================
// src/visual_scripting/visual_script.hpp — Visual scripting graph nodes.
//
// Alternativa visual a ARXScript: conectas nodos tipo Unreal Blueprints.
// Cada nodo tiene inputs/slots y se conecta con edges. Se compila a AST
// para ejecutarse con la VM o transpilarse a C++.
// ==============================================================================
#pragma once

#include "core/types.hpp"
#include "core/variant.hpp"
// #include "arxscript/ast.hpp"  // REMOVIDO: modulo arxscript eliminado

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace arx::vs {

// Pin — un conector de un nodo (input o output).
struct Pin {
    enum class Direction { Input, Output };
    enum class Type { Exec, Bool, Int, Float, String, Vector2, Vector3, Color, Object, Any };

    uint32_t  id;
    Direction direction;
    Type      type;
    std::string name;
    Variant  default_value;     // Para inputs con valor fijo.
};

// Edge — conexión entre dos pins.
struct Edge {
    uint32_t id;
    uint32_t from_pin;          // Pin de salida
    uint32_t to_pin;            // Pin de entrada
};

// Vox — nodo visual de un tipo específico.
struct Vox {
    enum class Kind {
        Event,            // OnStart, OnUpdate, OnClick, ...
        Variable,         // Get/Set variable
        Function,         // Call a función
        Operator,         // +, -, *, /, ==, ...
        Branch,           // If/Else
        Sequence,         // Ejecutar en orden
        ForLoop,          // For each
        WhileLoop,         // While
        Print,            // Debug print
        Literal,          // Constante
        MakeVector,       // Crear Vector2/3/4
        BreakVector,      // Descomponer vector
        Spawn,            // Instanciar objeto
        Destroy,          // Destruir objeto
        Custom            // Definido por el usuario
    };

    uint32_t id;
    Kind    kind;
    std::string title;
    std::string subtype;       // Ej: "OnStart" para Event, "+" para Operator
    Vector2  position;          // Posición en el canvas (para el editor visual)
    std::vector<Pin> inputs;
    std::vector<Pin> outputs;
};

// Graph — colección de nodos + edges = un script visual.
class Graph {
public:
    void add_node(Vox n);
    void remove_node(uint32_t id);
    Vox* get_node(uint32_t id);

    void add_edge(Edge e);
    void remove_edge(uint32_t id);
    std::vector<uint32_t> get_edges_from(uint32_t pin_id) const;
    std::vector<uint32_t> get_edges_to(uint32_t pin_id) const;

    const std::vector<Vox>& get_nodes() const { return nodes_; }
    const std::vector<Edge>& get_edges() const { return edges_; }

    // Compilar el graph a un AST de ARXScript (para transpilar a C++).
    // std::unique_ptr<arx::arxscript::Module> to_ast() const;  // REMOVIDO: arxscript eliminado

    // Serializar a JSON.
    std::string to_json() const;
    bool load_from_json(const std::string& json);

    // Stats.
    int node_count() const { return (int)nodes_.size(); }
    int edge_count() const { return (int)edges_.size(); }

private:
    std::vector<Vox> nodes_;
    std::vector<Edge> edges_;
    uint32_t next_id_ = 1;
};

// GraphRegistry — registry de graphs cargados.
class GraphRegistry {
public:
    static GraphRegistry& instance();
    std::shared_ptr<Graph> load(const std::string& path);
    bool save(const std::shared_ptr<Graph>& g, const std::string& path);

private:
    GraphRegistry() = default;
    std::unordered_map<std::string, std::shared_ptr<Graph>> cache_;
};

// NodeFactory — para crear nodos por tipo (lo usa el editor visual).
class NodeFactory {
public:
    static NodeFactory& instance();
    Vox create_event(const std::string& event_name);
    Vox create_operator(const std::string& op);
    Vox create_branch();
    Vox create_print();
    Vox create_literal(arx::Variant value);
    Vox create_make_vector(int dimensions);
    Vox create_function_call(const std::string& fn_name, int arg_count);

    std::vector<std::string> get_categories() const;
    std::vector<std::pair<std::string, std::string>> get_node_templates() const;
};

} // namespace arx::vs
