// ==============================================================================
// src/render/postprocess/postprocess.hpp — Efectos de post-procesado.
// Bloom, FXAA, Vignette, Blur, Color Grading, Chromatic Aberration.
// ==============================================================================
#pragma once

#include "core/types.hpp"
#include "render/texture.hpp"

#include <memory>
#include <string>

namespace arx {

class Shader;
class Mesh;

// PostProcessEffect — base de efectos.
class PostProcessEffect {
public:
    virtual ~PostProcessEffect() = default;
    virtual bool init() = 0;
    virtual void apply(std::shared_ptr<Texture> input,
                        std::shared_ptr<Texture> output) = 0;
    virtual const char* name() const = 0;
    virtual void set_intensity(float i) { intensity_ = i; }
    float get_intensity() const { return intensity_; }

protected:
    float intensity_ = 1.0f;
    std::shared_ptr<Shader> shader_;
    std::shared_ptr<Mesh>   quad_;
};

// Bloom — realza zonas brillantes.
class BloomEffect : public PostProcessEffect {
public:
    bool init() override;
    void apply(std::shared_ptr<Texture> input, std::shared_ptr<Texture> output) override;
    const char* name() const override { return "Bloom"; }

    void set_threshold(float t) { threshold_ = t; }
    void set_radius(float r)    { radius_ = r; }

private:
    float threshold_ = 1.0f;
    float radius_    = 1.0f;
};

// FXAA — anti-aliasing rápido.
class FXAAEffect : public PostProcessEffect {
public:
    bool init() override;
    void apply(std::shared_ptr<Texture> input, std::shared_ptr<Texture> output) override;
    const char* name() const override { return "FXAA"; }
};

// Vignette — oscurece los bordes.
class VignetteEffect : public PostProcessEffect {
public:
    bool init() override;
    void apply(std::shared_ptr<Texture> input, std::shared_ptr<Texture> output) override;
    const char* name() const override { return "Vignette"; }

    void set_color(Color c) { color_ = c; }
    void set_softness(float s) { softness_ = s; }

private:
    Color color_ = Color(0, 0, 0, 1);
    float softness_ = 0.5f;
};

// Blur — desenfoque gaussiano.
class BlurEffect : public PostProcessEffect {
public:
    bool init() override;
    void apply(std::shared_ptr<Texture> input, std::shared_ptr<Texture> output) override;
    const char* name() const override { return "Blur"; }

    void set_radius(float r) { radius_ = r; }

private:
    float radius_ = 3.0f;
};

// ColorGrading — aplica LUT o ajustes de color.
class ColorGradingEffect : public PostProcessEffect {
public:
    bool init() override;
    void apply(std::shared_ptr<Texture> input, std::shared_ptr<Texture> output) override;
    const char* name() const override { return "ColorGrading"; }

    void set_brightness(float b) { brightness_ = b; }
    void set_contrast(float c)   { contrast_ = c; }
    void set_saturation(float s) { saturation_ = s; }

private:
    float brightness_ = 0.0f;
    float contrast_   = 1.0f;
    float saturation_ = 1.0f;
};

// ChromaticAberration — separa canales RGB en bordes.
class ChromaticAberrationEffect : public PostProcessEffect {
public:
    bool init() override;
    void apply(std::shared_ptr<Texture> input, std::shared_ptr<Texture> output) override;
    const char* name() const override { return "ChromaticAberration"; }

    void set_amount(float a) { amount_ = a; }

private:
    float amount_ = 0.005f;
};

// PostProcessStack — cadena de efectos.
class PostProcessStack {
public:
    void add_effect(std::shared_ptr<PostProcessEffect> e) { effects_.push_back(std::move(e)); }
    void clear() { effects_.clear(); }
    void apply(std::shared_ptr<Texture> input, std::shared_ptr<Texture> output);
    size_t effect_count() const { return effects_.size(); }

private:
    std::vector<std::shared_ptr<PostProcessEffect>> effects_;
};

// Shaders GLSL embebidos.
namespace postprocess_shaders {
    extern const char* const bloom_vertex;
    extern const char* const bloom_fragment;
    extern const char* const fxaa_vertex;
    extern const char* const fxaa_fragment;
    extern const char* const vignette_vertex;
    extern const char* const vignette_fragment;
    extern const char* const blur_vertex;
    extern const char* const blur_fragment;
    extern const char* const color_grading_vertex;
    extern const char* const color_grading_fragment;
    extern const char* const chromatic_vertex;
    extern const char* const chromatic_fragment;
}

} // namespace arx
