// ==============================================================================
// src/runtime/zen_gaming_impl.cpp — Implementación de las funciones arx_* 
// que Zen llama via FFI.
//
// Este archivo se compila como parte de arx_core y provee las implementaciones
// de las funciones declaradas en zen_gaming_api.hpp.
// ==============================================================================
#include "core/types.hpp"
#include "core/logging.hpp"
#include "os/os.hpp"
#include "input/input.hpp"
#include "audio/audio_server.hpp"
#include "render/renderer.hpp"
#include "scene/scene_tree.hpp"

#include <cmath>
#include <cstring>

namespace arx {
// Globals accesibles desde las funciones C
static SceneTree* g_tree = nullptr;
static Renderer* g_renderer = nullptr;
static OS* g_os = nullptr;
static double g_delta = 0.016;
static bool g_quit = false;

void set_zen_globals(SceneTree* tree, Renderer* r, OS* os, double delta) {
    g_tree = tree;
    g_renderer = r;
    g_os = os;
    g_delta = delta;
}

void set_zen_delta(double delta) { g_delta = delta; }
bool is_zen_quit() { return g_quit; }
} // namespace arx

// === C ABI implementations ===
extern "C" {

// --- INPUT: teclado ---
int arx_key_down(int key) {
    return arx::Input::is_key_down(static_cast<arx::Key>(key)) ? 1 : 0;
}

int arx_key_pressed(int key) {
    return arx::Input::is_key_pressed(static_cast<arx::Key>(key)) ? 1 : 0;
}

int arx_key_released(int key) {
    return arx::Input::is_key_released(static_cast<arx::Key>(key)) ? 1 : 0;
}

// --- INPUT: mouse ---
int arx_mouse_down(int button) {
    return arx::Input::is_mouse_button_down(static_cast<arx::MouseButton>(button)) ? 1 : 0;
}

int arx_mouse_pressed(int button) {
    return arx::Input::is_mouse_button_pressed(static_cast<arx::MouseButton>(button)) ? 1 : 0;
}

double arx_mouse_x(void) {
    if (arx::g_os) {
        auto [x, y] = arx::g_os->get_mouse_position();
        return x;
    }
    return 0.0;
}

double arx_mouse_y(void) {
    if (arx::g_os) {
        auto [x, y] = arx::g_os->get_mouse_position();
        return y;
    }
    return 0.0;
}

double arx_mouse_wheel(void) {
    return arx::Input::get_mouse_wheel();
}

// --- INPUT: gamepad ---
int arx_gamepad_connected(int pad) {
    return arx::Input::is_gamepad_connected(pad) ? 1 : 0;
}

double arx_gamepad_axis(int pad, int axis) {
    return arx::Input::get_gamepad_axis(pad, axis);
}

int arx_gamepad_button(int pad, int button) {
    return arx::Input::is_gamepad_button_down(pad, button) ? 1 : 0;
}

// --- AUDIO ---
void arx_audio_play(const char* path) {
    auto stream = arx::AudioServer::instance().load(path);
    if (stream) {
        arx::AudioServer::instance().play(stream, "master", false);
    }
}

void arx_audio_stop(void) {
    arx::AudioServer::instance().set_master_volume(0);
}

void arx_audio_set_volume(double vol) {
    arx::AudioServer::instance().set_master_volume(vol);
}

double arx_audio_get_volume(void) {
    return arx::AudioServer::instance().get_master_volume();
}

// --- PHYSICS ---
int arx_physics_raycast(double x, double y, double z,
                          double dx, double dy, double dz,
                          double max_dist) {
    if (!arx::g_tree) return 0;
    auto* ps = arx::g_tree->get_physics_server();
    if (!ps) return 0;
    arx::Vector3 from(x, y, z);
    arx::Vector3 dir(dx, dy, dz);
    // Simplificado: retornar 0 (no hit)
    return 0;
}

void arx_physics_set_gravity(double x, double y, double z) {
    if (!arx::g_tree) return;
    auto* ps = arx::g_tree->get_physics_server();
    if (!ps) return;
    auto space = arx::g_tree->get_default_space();
    ps->space_set_gravity(space, arx::Vector3(x, y, z));
}

// --- RENDER 2D ---
void arx_draw_rect(double x, double y, double w, double h,
                     double r, double g, double b, double a) {
    if (!arx::g_renderer) return;
    arx::g_renderer->draw_rect_fill(x, y, w, h, r, g, b, a);
}

void arx_draw_circle(double x, double y, double radius,
                       double r, double g, double b, double a) {
    if (!arx::g_renderer) return;
    arx::g_renderer->draw_circle_fill(x, y, radius, r, g, b, a);
}

void arx_draw_text(const char* text, double x, double y,
                     double r, double g, double b) {
    if (!arx::g_renderer || !text) return;
    arx::g_renderer->draw_text(text, x, y, r, g, b);
}

void arx_draw_line(double x1, double y1, double x2, double y2,
                     double r, double g, double b) {
    if (!arx::g_renderer) return;
    arx::g_renderer->draw_line(x1, y1, x2, y2, r, g, b);
}

// --- ENGINE ---
double arx_get_fps(void) {
    if (arx::g_delta > 0) return 1.0 / arx::g_delta;
    return 60.0;
}

double arx_get_delta(void) {
    return arx::g_delta;
}

void arx_quit(void) {
    arx::g_quit = true;
    if (arx::g_tree) arx::g_tree->quit();
}

// --- ASSETS ---
// Cargar un .aex como asset pack (lazy loading)
static std::vector<std::string> g_loaded_aex;

int arx_aex_cargar(const char* path) {
    if (!path) return 0;
    // TODO: usar aex_open para cargar el .aex y registrar sus assets
    g_loaded_aex.push_back(path);
    return 1;
}

int arx_aex_descargar(const char* path) {
    if (!path) return 0;
    auto it = std::find(g_loaded_aex.begin(), g_loaded_aex.end(), path);
    if (it != g_loaded_aex.end()) {
        g_loaded_aex.erase(it);
        return 1;
    }
    return 0;
}

int arx_aex_cargado(const char* path) {
    if (!path) return 0;
    return std::find(g_loaded_aex.begin(), g_loaded_aex.end(), path) != g_loaded_aex.end() ? 1 : 0;
}

} // extern "C"
