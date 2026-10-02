#include "render/renderer.hpp"
// ==============================================================================
// src/render/pipeline/render_pipeline.cpp
// ==============================================================================
#include "render_pipeline.hpp"
#include "core/logging.hpp"
#include "scene/3d/vox_mesh_instance_3d.hpp"

#include <algorithm>
#include <cmath>

namespace arx {

// ===================== ForwardPipeline ======================================
bool ForwardPipeline::init(int w, int h) {
    width_ = w; height_ = h;
    ARX_LOG_INFO("ForwardPipeline inicializado ({}x{})", w, h);
    return true;
}

void ForwardPipeline::resize(int w, int h) {
    width_ = w; height_ = h;
}

void ForwardPipeline::render(const RenderData& data) {
    // 1. Shadow pass: renderizar depth desde cada luz direccional.
    // (stub: en una impl completa se usaría un shadow map FBO).

    // 2. Geometry pass: para cada renderable, draw con su shader.
    auto r_up = Renderer::create(Renderer::Backend::OpenGL); Renderer* r = r_up.get(); (void)r;
    r->begin_frame();
    r->set_clear_color(data.clear_color);
    r->clear(true, true);
    r->set_ambient_light(data.ambient_color, data.ambient_energy);

    for (auto* light : data.lights) {
        if (light->get_type() == VoxLight3D::Type::Directional) {
            r->add_directional_light(light->get_global_position(),
                                       light->get_color(), light->get_energy());
        } else if (light->get_type() == VoxLight3D::Type::Omni) {
            r->add_point_light(light->get_global_position(),
                                 light->get_color(), light->get_energy(),
                                 light->get_range());
        }
    }

    for (const auto& renderable : data.renderables) {
        if (renderable.transparent) continue;  // Los transparentes van después.
        Matrix4 view = Matrix4(1.0f);
        Matrix4 proj = Matrix4(1.0f);
        if (data.camera) {
            // Stub: en una impl real se obtendría del camera.
        }
        r->draw_mesh(renderable.mesh, renderable.shader,
                     renderable.transform, view, proj);
    }

    // 3. Transparent pass (back to front).
    // (stub)

    // 4. Post-process.
    // postprocess_.apply(scene_color_texture, final_texture);

    r->end_frame();
    delete r;  // Solo para el stub; en producción sería compartido.
}

void ForwardPipeline::shutdown() {}

// ===================== ForwardPlusPipeline ===================================
bool ForwardPlusPipeline::init(int w, int h) {
    width_ = w; height_ = h;
    resize(w, h);
    ARX_LOG_INFO("ForwardPlusPipeline inicializado ({}x{}, tile {})", w, h, tile_size_);
    return true;
}

void ForwardPlusPipeline::resize(int w, int h) {
    width_ = w; height_ = h;
    tile_count_x_ = (w + tile_size_ - 1) / tile_size_;
    tile_count_y_ = (h + tile_size_ - 1) / tile_size_;
    tile_light_indices_.resize(tile_count_x_ * tile_count_y_);
}

void ForwardPlusPipeline::render(const RenderData& data) {
    // 1. Cull lights per tile.
    for (auto& tile : tile_light_indices_) tile.clear();
    for (size_t i = 0; i < data.lights.size(); ++i) {
        // Simplificación: asignar cada luz a todos los tiles.
        // En una impl real se haría frustum/sphere culling por tile.
        for (auto& tile : tile_light_indices_) tile.push_back((uint32_t)i);
    }

    // 2. Forward pass usando las light lists por tile.
    ForwardPipeline fwd;
    fwd.init(width_, height_);
    fwd.render(data);
    fwd.shutdown();
}

void ForwardPlusPipeline::shutdown() {}

// ===================== DeferredPipeline ======================================
bool DeferredPipeline::init(int w, int h) {
    width_ = w; height_ = h;
    ARX_LOG_INFO("DeferredPipeline inicializado ({}x{})", w, h);
    return true;
}

void DeferredPipeline::resize(int w, int h) {
    width_ = w; height_ = h;
}

void DeferredPipeline::render(const RenderData& data) {
    // 1. Geometry pass: renderizar todos los renderables al G-Buffer.
    //    G-Buffer attachments:
    //      - RT0: albedo.rgb + metallic.r
    //      - RT1: normal.rgb + roughness.r
    //      - RT2: emission.rgb
    //      - Depth

    // 2. Lighting pass: para cada pixel, leer G-Buffer y aplicar luces.
    //    Single fullscreen quad con shader que muestrea el G-Buffer.

    // 3. Transparent pass: forward-render transparent objects.

    // 4. Post-process.

    // Stub: usar Forward por ahora.
    ForwardPipeline fwd;
    fwd.init(width_, height_);
    fwd.render(data);
    fwd.shutdown();
}

void DeferredPipeline::shutdown() {}

// ===================== PipelineRegistry =====================================
PipelineRegistry& PipelineRegistry::instance() {
    static PipelineRegistry r;
    return r;
}

void PipelineRegistry::register_pipeline(std::shared_ptr<RenderPipeline> p) {
    pipelines_.push_back(std::move(p));
}

std::shared_ptr<RenderPipeline> PipelineRegistry::get_pipeline(const std::string& name) {
    for (auto& p : pipelines_) {
        if (p->name() == name) return p;
    }
    return nullptr;
}

std::vector<std::string> PipelineRegistry::get_pipeline_names() const {
    std::vector<std::string> out;
    for (const auto& p : pipelines_) out.push_back(p->name());
    return out;
}

// ===================== PipelineFactory ======================================
std::shared_ptr<RenderPipeline> PipelineFactory::create(const std::string& name) {
    if (name == "Forward")    return std::make_shared<ForwardPipeline>();
    if (name == "Forward+")   return std::make_shared<ForwardPlusPipeline>();
    if (name == "Deferred")   return std::make_shared<DeferredPipeline>();
    ARX_LOG_WARN("PipelineFactory: pipeline '{}' no existe, usando Forward", name);
    return std::make_shared<ForwardPipeline>();
}

} // namespace arx
