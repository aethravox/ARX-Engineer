// ==============================================================================
// src/render/postprocess/postprocess.cpp — Implementación + shaders GLSL.
// ==============================================================================
#include "postprocess.hpp"
#include "render/renderer.hpp"
#include "core/logging.hpp"

namespace arx::postprocess_shaders {

// Vertex shader común para fullscreen quad.
const char* const bloom_vertex = R"(
    #version 330 core
    layout(location=0) in vec2 a_pos;
    layout(location=1) in vec2 a_uv;
    out vec2 v_uv;
    void main() {
        v_uv = a_uv;
        gl_Position = vec4(a_pos, 0.0, 1.0);
    }
)";

const char* const bloom_fragment = R"(
    #version 330 core
    in vec2 v_uv;
    out vec4 frag;
    uniform sampler2D u_texture;
    uniform float u_threshold = 1.0;
    uniform float u_intensity = 1.0;
    void main() {
        vec3 c = texture(u_texture, v_uv).rgb;
        float bright = max(c.r, max(c.g, c.b));
        if (bright > u_threshold) {
            frag = vec4(c * u_intensity, 1.0);
        } else {
            frag = vec4(0.0, 0.0, 0.0, 1.0);
        }
    }
)";

const char* const fxaa_vertex = bloom_vertex;

const char* const fxaa_fragment = R"(
    #version 330 core
    in vec2 v_uv;
    out vec4 frag;
    uniform sampler2D u_texture;
    uniform vec2 u_texel_size;
    // FXAA 3.11 Quality (simplificado).
    void main() {
        vec3 rgbNW = texture(u_texture, v_uv + vec2(-1, -1) * u_texel_size).rgb;
        vec3 rgbNE = texture(u_texture, v_uv + vec2( 1, -1) * u_texel_size).rgb;
        vec3 rgbSW = texture(u_texture, v_uv + vec2(-1,  1) * u_texel_size).rgb;
        vec3 rgbSE = texture(u_texture, v_uv + vec2( 1,  1) * u_texel_size).rgb;
        vec3 rgbM  = texture(u_texture, v_uv).rgb;
        vec3 luma = vec3(0.299, 0.587, 0.114);
        float lNW = dot(rgbNW, luma);
        float lNE = dot(rgbNE, luma);
        float lSW = dot(rgbSW, luma);
        float lSE = dot(rgbSE, luma);
        float lM  = dot(rgbM,  luma);
        float lMin = min(lM, min(min(lNW, lNE), min(lSW, lSE)));
        float lMax = max(lM, max(max(lNW, lNE), max(lSW, lSE)));
        if (lMax - lMin < 0.05) { frag = vec4(rgbM, 1.0); return; }
        frag = vec4((rgbNW + rgbNE + rgbSW + rgbSE + rgbM) * 0.2, 1.0);
    }
)";

const char* const vignette_vertex = bloom_vertex;

const char* const vignette_fragment = R"(
    #version 330 core
    in vec2 v_uv;
    out vec4 frag;
    uniform sampler2D u_texture;
    uniform vec4  u_color = vec4(0,0,0,1);
    uniform float u_softness = 0.5;
    uniform float u_intensity = 1.0;
    void main() {
        vec4 c = texture(u_texture, v_uv);
        vec2 d = v_uv - vec2(0.5);
        float dist = length(d);
        float vig = smoothstep(u_softness, 0.8, dist);
        frag = mix(c, u_color, vig * u_intensity);
    }
)";

const char* const blur_vertex = bloom_vertex;

const char* const blur_fragment = R"(
    #version 330 core
    in vec2 v_uv;
    out vec4 frag;
    uniform sampler2D u_texture;
    uniform vec2 u_texel_size;
    uniform float u_radius = 3.0;
    void main() {
        vec4 sum = vec4(0);
        float total = 0;
        for (int x = -3; x <= 3; ++x) {
            for (int y = -3; y <= 3; ++y) {
                vec2 off = vec2(x, y) * u_texel_size * u_radius;
                float w = exp(-float(x*x + y*y) / 9.0);
                sum += texture(u_texture, v_uv + off) * w;
                total += w;
            }
        }
        frag = sum / total;
    }
)";

const char* const color_grading_vertex = bloom_vertex;

const char* const color_grading_fragment = R"(
    #version 330 core
    in vec2 v_uv;
    out vec4 frag;
    uniform sampler2D u_texture;
    uniform float u_brightness = 0.0;
    uniform float u_contrast = 1.0;
    uniform float u_saturation = 1.0;
    void main() {
        vec4 c = texture(u_texture, v_uv);
        c.rgb = (c.rgb - 0.5) * u_contrast + 0.5 + u_brightness;
        float l = dot(c.rgb, vec3(0.299, 0.587, 0.114));
        c.rgb = mix(vec3(l), c.rgb, u_saturation);
        frag = c;
    }
)";

const char* const chromatic_vertex = bloom_vertex;

const char* const chromatic_fragment = R"(
    #version 330 core
    in vec2 v_uv;
    out vec4 frag;
    uniform sampler2D u_texture;
    uniform float u_amount = 0.005;
    void main() {
        vec2 dir = v_uv - vec2(0.5);
        float r = texture(u_texture, v_uv - dir * u_amount).r;
        float g = texture(u_texture, v_uv).g;
        float b = texture(u_texture, v_uv + dir * u_amount).b;
        frag = vec4(r, g, b, 1.0);
    }
)";

} // namespace arx::postprocess_shaders

namespace arx {

// ===================== Implementaciones (stubs) =============================
bool BloomEffect::init() {
    // En una impl completa: shader_ = renderer->create_shader(...)
    return true;
}

void BloomEffect::apply(std::shared_ptr<Texture> /*input*/,
                          std::shared_ptr<Texture> /*output*/) {
    // Aplicar shader bloom_vertex + bloom_fragment.
}

bool FXAAEffect::init()        { return true; }
void FXAAEffect::apply(std::shared_ptr<Texture>, std::shared_ptr<Texture>) {}

bool VignetteEffect::init()    { return true; }
void VignetteEffect::apply(std::shared_ptr<Texture>, std::shared_ptr<Texture>) {}

bool BlurEffect::init()        { return true; }
void BlurEffect::apply(std::shared_ptr<Texture>, std::shared_ptr<Texture>) {}

bool ColorGradingEffect::init(){ return true; }
void ColorGradingEffect::apply(std::shared_ptr<Texture>, std::shared_ptr<Texture>) {}

bool ChromaticAberrationEffect::init() { return true; }
void ChromaticAberrationEffect::apply(std::shared_ptr<Texture>, std::shared_ptr<Texture>) {}

void PostProcessStack::apply(std::shared_ptr<Texture> input,
                                std::shared_ptr<Texture> output) {
    if (effects_.empty()) { return; }
    auto current = input;
    for (auto& e : effects_) {
        auto next = output;
        e->apply(current, next);
        current = next;
    }
}

} // namespace arx
