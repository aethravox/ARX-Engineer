// ==============================================================================
// src/render/pipeline/render_pipeline.hpp — Pipelines de render.
//
// Forward / Forward+ / Deferred. Cada uno con sus passes:
//   - Shadow pass
//   - Geometry pass (deferred: G-Buffer)
//   - Lighting pass (deferred)
//   - Forward pass (forward+: tile culling)
//   - Transparent pass
//   - Post-process pass
// ==============================================================================
#pragma once

#include "core/types.hpp"
#include "render/renderer.hpp"
#include "render/postprocess/postprocess.hpp"

#include <memory>
#include <vector>

namespace arx {

class VoxCamera3D;
class VoxLight3D;

// RenderPipeline — base de pipelines.
class RenderPipeline {
public:
    virtual ~RenderPipeline() = default;
    virtual bool init(int width, int height) = 0;
    virtual void resize(int width, int height) = 0;
    virtual void render(const struct RenderData& data) = 0;
    virtual void shutdown() = 0;

    virtual std::string name() const = 0;

    // Post-process stack (común a todos los pipelines).
    PostProcessStack& get_postprocess_stack() { return postprocess_; }

protected:
    PostProcessStack postprocess_;
    int width_  = 1280;
    int height_ = 720;
};

// RenderData — todo lo que un pipeline necesita para renderizar un frame.
struct RenderData {
    VoxCamera3D*            camera  = nullptr;
    std::vector<VoxLight3D*> lights;
    std::vector<struct Renderable> renderables;
    Color  ambient_color  = Color::white;
    float  ambient_energy = 0.3f;
    Color  clear_color    = Color(0.05f, 0.05f, 0.08f, 1.0f);
    float  fog_density    = 0.0f;
    Color  fog_color      = Color::gray;
};

struct Renderable {
    std::shared_ptr<Mesh>   mesh;
    std::shared_ptr<Shader> shader;
    Transform3D             transform;
    Color                   albedo = Color::white;
    std::shared_ptr<Texture> texture;
    bool                    transparent = false;
    bool                    cast_shadow = true;
    bool                    receive_shadow = true;
};

// ForwardPipeline — render directo (1 pass por objeto).
class ForwardPipeline : public RenderPipeline {
public:
    bool init(int w, int h) override;
    void resize(int w, int h) override;
    void render(const RenderData& data) override;
    void shutdown() override;
    std::string name() const override { return "Forward"; }
};

// ForwardPlusPipeline — Forward + tile-based light culling.
// Soporta miles de luces dinámicas dividiendo la pantalla en tiles.
class ForwardPlusPipeline : public RenderPipeline {
public:
    bool init(int w, int h) override;
    void resize(int w, int h) override;
    void render(const RenderData& data) override;
    void shutdown() override;
    std::string name() const override { return "Forward+"; }

    void set_tile_size(int s) { tile_size_ = s; }
    int  get_tile_size() const { return tile_size_; }
    int  get_tile_count_x() const { return tile_count_x_; }
    int  get_tile_count_y() const { return tile_count_y_; }

private:
    int tile_size_    = 16;
    int tile_count_x_ = 0;
    int tile_count_y_ = 0;
    std::vector<std::vector<uint32_t>> tile_light_indices_;   // Por tile, lista de light indices.
};

// DeferredPipeline — G-Buffer + lighting pass.
class DeferredPipeline : public RenderPipeline {
public:
    bool init(int w, int h) override;
    void resize(int w, int h) override;
    void render(const RenderData& data) override;
    void shutdown() override;
    std::string name() const override { return "Deferred"; }

    // G-Buffer attachments: albedo, normal, material (metallic/roughness), depth.
    void set_gbuffer_format(uint32_t format) { gbuffer_format_ = format; }

private:
    uint32_t gbuffer_format_ = 0;
    // En una impl completa: 4 framebuffer textures + 1 depth.
};

// PipelineRegistry — registry de pipelines disponibles.
class PipelineRegistry {
public:
    static PipelineRegistry& instance();
    void register_pipeline(std::shared_ptr<RenderPipeline> p);
    std::shared_ptr<RenderPipeline> get_pipeline(const std::string& name);
    std::vector<std::string> get_pipeline_names() const;

private:
    PipelineRegistry() = default;
    std::vector<std::shared_ptr<RenderPipeline>> pipelines_;
};

// PipelineFactory — crea pipelines por nombre.
class PipelineFactory {
public:
    static std::shared_ptr<RenderPipeline> create(const std::string& name);
};

} // namespace arx
