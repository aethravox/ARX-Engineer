// ==============================================================================
// src/editor/visual_scripting/vs_editor.hpp — Editor visual de scripts.
// Canvas interactivo para crear graphs de visual scripting.
// ==============================================================================
#pragma once

#include "visual_scripting/visual_script.hpp"
#include "core/types.hpp"

#include <memory>
#include <string>
#include <vector>

namespace arx {

class VSEditor {
public:
    VSEditor();

    void set_graph(std::shared_ptr<vs::Graph> g) { graph_ = g; }
    std::shared_ptr<vs::Graph> get_graph() const { return graph_; }

    // Render UI con Dear ImGui.
    void render();

    // Interacciones.
    void add_node_at(const vs::Vox& node_template, Vector2 position);
    void delete_selected();
    void select_node(uint32_t id);
    void clear_selection();

    // Conexiones.
    void start_connection(uint32_t from_pin);
    void complete_connection(uint32_t to_pin);
    void cancel_connection();

    // Búsqueda de nodos (palette).
    void show_node_palette(bool show) { show_palette_ = show; }

    // Save/load.
    bool save(const std::string& path);
    bool load(const std::string& path);

private:
    void render_canvas();
    void render_node(const vs::Vox& n);
    void render_pin(const vs::Pin& p, const vs::Vox& owner);
    void render_node_palette();
    void render_properties_panel();

    std::shared_ptr<vs::Graph> graph_;
    std::vector<uint32_t>      selected_nodes_;
    bool                       show_palette_ = false;
    uint32_t                   connecting_from_pin_ = 0;
    bool                       connecting_ = false;
    Vector2                    mouse_pos_;
    Vector2                    canvas_offset_{0, 0};
    float                      canvas_zoom_ = 1.0f;

    // Drag state.
    bool                       dragging_ = false;
    Vector2                    drag_start_;
    uint32_t                   dragged_node_ = 0;
};

// ShaderGraph — variant de visual scripting para crear shaders visuales.
class ShaderGraph {
public:
    struct SNode {
        enum class Kind {
            Output,        // Vertex color / frag color
            Texture,       // Sample texture
            Color,         // Constant color
            Vector,        // Constant vector
            Scalar,        // Constant float
            Add, Sub, Mul, Div,
            Lerp, Saturate, Sine, Cosine, Pow, Sqrt,
            Time, UV, Normal, VertexColor, WorldPosition,
            Fresnel, Distance, Dot, Cross, Normalize,
            Mask, Combine, Swizzle,
            Custom
        };

        uint32_t    id;
        Kind        kind;
        std::string title;
        Vector2     position;
        std::vector<vs::Pin> inputs;
        std::vector<vs::Pin> outputs;
    };

    void add_node(SNode n);
    void remove_node(uint32_t id);
    void add_edge(vs::Edge e);

    // Generar código GLSL del vertex + fragment shader.
    std::string generate_vertex_shader() const;
    std::string generate_fragment_shader() const;

    const std::vector<SNode>& get_nodes() const { return nodes_; }

private:
    std::vector<SNode> nodes_;
    std::vector<vs::Edge> edges_;
};

} // namespace arx
