// ==============================================================================
#include "core/variant.hpp"
// src/visual_scripting/visual_script.cpp
// ==============================================================================
#include "visual_script.hpp"
#include "json/json.hpp"
#include "core/logging.hpp"

#include <fstream>
#include <sstream>

namespace arx::vs {

void Graph::add_node(Vox n) {
    if (n.id == 0) n.id = next_id_++;
    else if (n.id >= next_id_) next_id_ = n.id + 1;
    nodes_.push_back(std::move(n));
}

void Graph::remove_node(uint32_t id) {
    // Eliminar edges conectados.
    edges_.erase(
        std::remove_if(edges_.begin(), edges_.end(),
            [id](const Edge& e) {
                // Simplificación: no sabemos qué pin pertenece a qué nodo aquí.
                // En una impl completa se miraría node->inputs/outputs.
                (void)id; (void)e;
                return false;
            }),
        edges_.end());
    nodes_.erase(
        std::remove_if(nodes_.begin(), nodes_.end(),
            [id](const Vox& n) { return n.id == id; }),
        nodes_.end());
}

Vox* Graph::get_node(uint32_t id) {
    for (auto& n : nodes_) if (n.id == id) return &n;
    return nullptr;
}

void Graph::add_edge(Edge e) {
    if (e.id == 0) e.id = next_id_++;
    edges_.push_back(std::move(e));
}

void Graph::remove_edge(uint32_t id) {
    edges_.erase(
        std::remove_if(edges_.begin(), edges_.end(),
            [id](const Edge& e) { return e.id == id; }),
        edges_.end());
}

std::vector<uint32_t> Graph::get_edges_from(uint32_t pin_id) const {
    std::vector<uint32_t> out;
    for (const auto& e : edges_) if (e.from_pin == pin_id) out.push_back(e.id);
    return out;
}

std::vector<uint32_t> Graph::get_edges_to(uint32_t pin_id) const {
    std::vector<uint32_t> out;
    for (const auto& e : edges_) if (e.to_pin == pin_id) out.push_back(e.id);
    return out;
}

std::string Graph::to_json() const {
    json::Object root;
    json::Array nodes_arr;
    for (const auto& n : nodes_) {
        json::Object node_obj;
        node_obj["id"]       = (int64_t)n.id;
        node_obj["kind"]     = (int64_t)n.kind;
        node_obj["title"]    = n.title;
        node_obj["subtype"]  = n.subtype;
        node_obj["position"] = json::Array{ (double)n.position.x, (double)n.position.y };
        nodes_arr.push_back(json::Value(node_obj));
    }
    root["nodes"] = json::Value(std::move(nodes_arr));
    json::Array edges_arr;
    for (const auto& e : edges_) {
        json::Object edge_obj;
        edge_obj["id"]        = (int64_t)e.id;
        edge_obj["from_pin"]  = (int64_t)e.from_pin;
        edge_obj["to_pin"]    = (int64_t)e.to_pin;
        edges_arr.push_back(json::Value(edge_obj));
    }
    root["edges"] = json::Value(std::move(edges_arr));
    return json::serialize(json::Value(std::move(root)), true);
}

bool Graph::load_from_json(const std::string& jstr) {
    std::string err;
    json::Value v = json::parse(jstr, &err);
    if (!err.empty()) return false;
    if (!v.is_object()) return false;

    nodes_.clear(); edges_.clear();

    if (v.has("nodes") && v["nodes"].is_array()) {
        for (const auto& n : v["nodes"].as_array()) {
            Vox node;
            node.id       = (uint32_t)n["id"].as_int();
            node.kind     = (Vox::Kind)n["kind"].as_int();
            node.title    = n["title"].as_string();
            node.subtype  = n["subtype"].as_string();
            if (n["position"].is_array() && n["position"].size() >= 2) {
                node.position = Vector2((float)n["position"][0].as_float(),
                                        (float)n["position"][1].as_float());
            }
            nodes_.push_back(std::move(node));
        }
    }
    if (v.has("edges") && v["edges"].is_array()) {
        for (const auto& e : v["edges"].as_array()) {
            Edge edge;
            edge.id       = (uint32_t)e["id"].as_int();
            edge.from_pin = (uint32_t)e["from_pin"].as_int();
            edge.to_pin   = (uint32_t)e["to_pin"].as_int();
            edges_.push_back(std::move(edge));
        }
    }
    return true;
}

std::unique_ptr<arx::arxscript::Module> Graph::to_ast() const {
    auto mod = std::make_unique<arx::arxscript::Module>();
    // Convertir nodos → AST nodes. Stub: solo imprimimos qué haríamos.
    ARX_LOG_INFO("VS Graph → AST: {} nodes, {} edges", nodes_.size(), edges_.size());
    return mod;
}

// ===================== GraphRegistry =========================================
GraphRegistry& GraphRegistry::instance() {
    static GraphRegistry r;
    return r;
}

std::shared_ptr<Graph> GraphRegistry::load(const std::string& path) {
    auto it = cache_.find(path);
    if (it != cache_.end()) return it->second;
    std::ifstream f(path);
    if (!f) return nullptr;
    std::stringstream ss; ss << f.rdbuf();
    auto g = std::make_shared<Graph>();
    if (!g->load_from_json(ss.str())) return nullptr;
    cache_[path] = g;
    return g;
}

bool GraphRegistry::save(const std::shared_ptr<Graph>& g, const std::string& path) {
    std::ofstream f(path);
    if (!f) return false;
    f << g->to_json();
    return true;
}

// ===================== NodeFactory ===========================================
NodeFactory& NodeFactory::instance() {
    static NodeFactory f;
    return f;
}

Vox NodeFactory::create_event(const std::string& event_name) {
    Vox n;
    n.kind = Vox::Kind::Event;
    n.title = event_name;
    n.subtype = event_name;
    Pin out{1, Pin::Direction::Output, Pin::Type::Exec, "out"};
    n.outputs.push_back(out);
    return n;
}

Vox NodeFactory::create_operator(const std::string& op) {
    Vox n;
    n.kind = Vox::Kind::Operator;
    n.title = op;
    n.subtype = op;
    Pin a{1, Pin::Direction::Input,  Pin::Type::Any, "A"};
    Pin b{2, Pin::Direction::Input,  Pin::Type::Any, "B"};
    Pin r{3, Pin::Direction::Output, Pin::Type::Any, "result"};
    n.inputs = {a, b};
    n.outputs = {r};
    return n;
}

Vox NodeFactory::create_branch() {
    Vox n;
    n.kind = Vox::Kind::Branch;
    n.title = "Branch";
    Pin cond{1, Pin::Direction::Input,  Pin::Type::Bool, "condition"};
    Pin in  {2, Pin::Direction::Input,  Pin::Type::Exec, "in"};
    Pin yes {3, Pin::Direction::Output, Pin::Type::Exec, "true"};
    Pin no  {4, Pin::Direction::Output, Pin::Type::Exec, "false"};
    n.inputs = {cond, in};
    n.outputs = {yes, no};
    return n;
}

Vox NodeFactory::create_print() {
    Vox n;
    n.kind = Vox::Kind::Print;
    n.title = "Print";
    Pin in   {1, Pin::Direction::Input,  Pin::Type::Exec, "in"};
    Pin val  {2, Pin::Direction::Input,  Pin::Type::Any,  "value"};
    Pin out  {3, Pin::Direction::Output, Pin::Type::Exec, "out"};
    n.inputs = {in, val};
    n.outputs = {out};
    return n;
}

Vox NodeFactory::create_literal(arx::Variant value) {
    Vox n;
    n.kind = Vox::Kind::Literal;
    n.title = "Literal";
    Pin out{1, Pin::Direction::Output, Pin::Type::Any, "value"};
    out.default_value = value;
    n.outputs.push_back(out);
    return n;
}

Vox NodeFactory::create_make_vector(int dims) {
    Vox n;
    n.kind = Vox::Kind::MakeVector;
    n.title = "Make Vector" + std::to_string(dims);
    for (int i = 0; i < dims; ++i) {
        Pin p{(uint32_t)(i+1), Pin::Direction::Input, Pin::Type::Float,
              std::string(1, "xyzw"[i])};
        n.inputs.push_back(p);
    }
    Pin out{100, Pin::Direction::Output, Pin::Type::Vector3, "vector"};
    n.outputs.push_back(out);
    return n;
}

Vox NodeFactory::create_function_call(const std::string& fn_name, int arg_count) {
    Vox n;
    n.kind = Vox::Kind::Function;
    n.title = fn_name;
    n.subtype = fn_name;
    Pin in{0, Pin::Direction::Input, Pin::Type::Exec, "in"};
    n.inputs.push_back(in);
    for (int i = 0; i < arg_count; ++i) {
        Pin p{(uint32_t)(i+1), Pin::Direction::Input, Pin::Type::Any,
              "arg" + std::to_string(i)};
        n.inputs.push_back(p);
    }
    Pin out{200, Pin::Direction::Output, Pin::Type::Exec, "out"};
    Pin ret{201, Pin::Direction::Output, Pin::Type::Any, "return"};
    n.outputs = {out, ret};
    return n;
}

std::vector<std::string> NodeFactory::get_categories() const {
    return {"Events", "Flow Control", "Operators", "Math", "Variables",
            "Functions", "Debug", "Objects", "Vectors", "Custom"};
}

std::vector<std::pair<std::string, std::string>> NodeFactory::get_node_templates() const {
    return {
        {"Events/OnStart", "On Start"},
        {"Events/OnUpdate", "On Update"},
        {"Events/OnInput", "On Input"},
        {"Flow/Branch", "If/Else"},
        {"Flow/ForLoop", "For Loop"},
        {"Flow/WhileLoop", "While Loop"},
        {"Flow/Sequence", "Sequence"},
        {"Operators/Add", "Add (+)"},
        {"Operators/Sub", "Subtract (-)"},
        {"Operators/Mul", "Multiply (*)"},
        {"Operators/Div", "Divide (/)"},
        {"Operators/Equal", "Equal (==)"},
        {"Math/Sin", "Sin"},
        {"Math/Cos", "Cos"},
        {"Math/Sqrt", "Sqrt"},
        {"Math/Random", "Random"},
        {"Variables/Get", "Get Variable"},
        {"Variables/Set", "Set Variable"},
        {"Debug/Print", "Print"},
        {"Objects/Spawn", "Spawn Object"},
        {"Objects/Destroy", "Destroy Object"},
        {"Vectors/MakeVector2", "Make Vector2"},
        {"Vectors/MakeVector3", "Make Vector3"},
        {"Vectors/BreakVector", "Break Vector"},
    };
}

} // namespace arx::vs
