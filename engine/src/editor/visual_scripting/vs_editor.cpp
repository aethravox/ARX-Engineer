// ==============================================================================
// src/editor/visual_scripting/vs_editor.cpp
// ==============================================================================
#include "vs_editor.hpp"
#include "core/logging.hpp"

#include <imgui.h>
#include <sstream>
#include <algorithm>

namespace arx {

VSEditor::VSEditor() = default;

void VSEditor::render() {
    if (!ImGui::Begin("Visual Scripting")) {
        ImGui::End();
        return;
    }

    // Toolbar.
    if (ImGui::Button("Add Vox")) show_palette_ = !show_palette_;
    ImGui::SameLine();
    if (ImGui::Button("Delete"))   delete_selected();
    ImGui::SameLine();
    if (ImGui::Button("Save"))     { /* dialog */ }
    ImGui::SameLine();
    if (ImGui::Button("Load"))     { /* dialog */ }
    ImGui::SameLine();
    if (ImGui::Button("Compile")) {
        if (graph_) { /* to_ast() removido - arxscript eliminado */ }
    }

    ImGui::Separator();

    // Layout: canvas (centro) + palette (izq) + properties (der).
    float total_w = ImGui::GetContentRegionAvail().x;
    if (show_palette_) {
        ImGui::BeginChild("palette", ImVec2(200, 0), true);
        render_node_palette();
        ImGui::EndChild();
        ImGui::SameLine();
    }

    ImGui::BeginChild("canvas", ImVec2(ImGui::GetContentRegionAvail().x - 250, 0), true);
    render_canvas();
    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::BeginChild("properties", ImVec2(250, 0), true);
    render_properties_panel();
    ImGui::EndChild();

    ImGui::End();
}

void VSEditor::render_canvas() {
    if (!graph_) {
        ImGui::TextDisabled("(no graph loaded)");
        return;
    }

    ImVec2 origin = ImGui::GetCursorScreenPos();
    ImDrawList* dl = ImGui::GetWindowDrawList();

    // Grid.
    ImU32 grid_color = IM_COL32(40, 40, 50, 200);
    for (float x = 0; x < ImGui::GetContentRegionAvail().x; x += 20) {
        dl->AddLine(ImVec2(origin.x + x, origin.y),
                    ImVec2(origin.x + x, origin.y + ImGui::GetContentRegionAvail().y),
                    grid_color);
    }
    for (float y = 0; y < ImGui::GetContentRegionAvail().y; y += 20) {
        dl->AddLine(ImVec2(origin.x, origin.y + y),
                    ImVec2(origin.x + ImGui::GetContentRegionAvail().x, origin.y + y),
                    grid_color);
    }

    // Render nodos.
    for (const auto& n : graph_->get_nodes()) {
        render_node(n);
    }

    // Render edges.
    for (const auto& e : graph_->get_edges()) {
        // Stub: dibujar línea bezier entre los dos pins.
        // En una impl completa se buscaría la posición de cada pin.
        (void)e;
    }
}

void VSEditor::render_node(const vs::Vox& n) {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 pos(n.position.x, n.position.y);
    ImVec2 size(160, 80);

    ImU32 bg_color = IM_COL32(30, 30, 35, 240);
    ImU32 border = IM_COL32(107, 54, 217, 255);  // ARX purple
    bool selected = std::find(selected_nodes_.begin(), selected_nodes_.end(), n.id)
                    != selected_nodes_.end();
    if (selected) border = IM_COL32(26, 217, 235, 255);  // ARX cyan

    dl->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), bg_color, 4.0f);
    dl->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), border, 4.0f, 0, 2.0f);

    // Title bar.
    dl->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + 20), border, 4.0f);
    dl->AddText(ImVec2(pos.x + 6, pos.y + 4), IM_COL32(255,255,255,255), n.title.c_str());

    // Inputs.
    for (size_t i = 0; i < n.inputs.size(); ++i) {
        ImVec2 p(pos.x - 4, pos.y + 30 + (float)i * 16);
        dl->AddCircleFilled(p, 5, IM_COL32(180, 180, 180, 255));
        dl->AddText(ImVec2(pos.x + 8, pos.y + 26 + (float)i * 16),
                    IM_COL32(220, 220, 220, 255), n.inputs[i].name.c_str());
    }
    // Outputs.
    for (size_t i = 0; i < n.outputs.size(); ++i) {
        ImVec2 p(pos.x + size.x + 4, pos.y + 30 + (float)i * 16);
        dl->AddCircleFilled(p, 5, IM_COL32(180, 180, 180, 255));
        const char* txt = n.outputs[i].name.c_str();
        ImVec2 ts = ImGui::CalcTextSize(txt);
        dl->AddText(ImVec2(pos.x + size.x - ts.x - 8, pos.y + 26 + (float)i * 16),
                    IM_COL32(220, 220, 220, 255), txt);
    }
}

void VSEditor::render_pin(const vs::Pin& p, const vs::Vox& owner) {
    (void)p; (void)owner;
}

void VSEditor::render_node_palette() {
    ImGui::TextDisabled("Vox Palette");
    ImGui::Separator();

    auto& factory = vs::NodeFactory::instance();
    for (const auto& cat : factory.get_categories()) {
        if (ImGui::TreeNode(cat.c_str())) {
            for (const auto& [path, name] : factory.get_node_templates()) {
                if (path.find(cat) == 0) {
                    if (ImGui::Selectable(name.c_str())) {
                        // Crear nodo del tipo seleccionado.
                        vs::Vox n;
                        n.title = name;
                        n.subtype = path;
                        if (graph_) graph_->add_node(n);
                    }
                }
            }
            ImGui::TreePop();
        }
    }
}

void VSEditor::render_properties_panel() {
    ImGui::TextDisabled("Properties");
    ImGui::Separator();
    if (selected_nodes_.empty()) {
        ImGui::TextDisabled("(no node selected)");
        return;
    }
    ImGui::Text("Selected: %u nodes", (uint32_t)selected_nodes_.size());
    if (selected_nodes_.size() == 1 && graph_) {
        vs::Vox* n = graph_->get_node(selected_nodes_[0]);
        if (n) {
            char buf[256];
            std::snprintf(buf, sizeof(buf), "%s", n->title.c_str());
            if (ImGui::InputText("Title", buf, sizeof(buf))) n->title = buf;
            ImGui::InputFloat2("Position", &n->position.x);
        }
    }
}

void VSEditor::add_node_at(const vs::Vox& tmpl, Vector2 pos) {
    vs::Vox n = tmpl;
    n.position = pos;
    if (graph_) graph_->add_node(n);
}

void VSEditor::delete_selected() {
    if (!graph_) return;
    for (uint32_t id : selected_nodes_) graph_->remove_node(id);
    selected_nodes_.clear();
}

void VSEditor::select_node(uint32_t id) {
    selected_nodes_.clear();
    selected_nodes_.push_back(id);
}

void VSEditor::clear_selection() { selected_nodes_.clear(); }

void VSEditor::start_connection(uint32_t from_pin) {
    connecting_from_pin_ = from_pin;
    connecting_ = true;
}

void VSEditor::complete_connection(uint32_t to_pin) {
    if (!graph_ || !connecting_) return;
    vs::Edge e;
    e.from_pin = connecting_from_pin_;
    e.to_pin   = to_pin;
    graph_->add_edge(e);
    connecting_ = false;
}

void VSEditor::cancel_connection() { connecting_ = false; }

bool VSEditor::save(const std::string& path) {
    if (!graph_) return false;
    return vs::GraphRegistry::instance().save(graph_, path);
}

bool VSEditor::load(const std::string& path) {
    graph_ = vs::GraphRegistry::instance().load(path);
    return graph_ != nullptr;
}

// ===================== ShaderGraph ===========================================
void ShaderGraph::add_node(SNode n) {
    if (n.id == 0) n.id = (uint32_t)nodes_.size() + 1;
    nodes_.push_back(std::move(n));
}

void ShaderGraph::remove_node(uint32_t id) {
    nodes_.erase(std::remove_if(nodes_.begin(), nodes_.end(),
        [id](const SNode& n) { return n.id == id; }), nodes_.end());
}

void ShaderGraph::add_edge(vs::Edge e) { edges_.push_back(std::move(e)); }

std::string ShaderGraph::generate_vertex_shader() const {
    std::ostringstream ss;
    ss << "#version 330 core\n";
    ss << "layout(location=0) in vec3 a_pos;\n";
    ss << "layout(location=1) in vec2 a_uv;\n";
    ss << "layout(location=2) in vec3 a_normal;\n";
    ss << "out vec2 v_uv;\n";
    ss << "out vec3 v_normal;\n";
    ss << "uniform mat4 u_model, u_view, u_proj;\n";
    ss << "void main() {\n";
    ss << "    v_uv = a_uv;\n";
    ss << "    v_normal = mat3(u_model) * a_normal;\n";
    ss << "    gl_Position = u_proj * u_view * u_model * vec4(a_pos, 1.0);\n";
    ss << "}\n";
    return ss.str();
}

std::string ShaderGraph::generate_fragment_shader() const {
    std::ostringstream ss;
    ss << "#version 330 core\n";
    ss << "in vec2 v_uv;\n";
    ss << "in vec3 v_normal;\n";
    ss << "out vec4 frag;\n";
    ss << "uniform sampler2D u_texture;\n";
    ss << "void main() {\n";
    ss << "    vec4 c = texture(u_texture, v_uv);\n";
    ss << "    frag = c;\n";
    ss << "}\n";
    return ss.str();
}

} // namespace arx
