// ==============================================================================
// src/editor/preview/preview.hpp — Vista previa en vivo del editor.
//
// Permite ejecutar el juego dentro del editor sin tener que exportar.
// Maneja:
//   - Spawn del SceneTree en un hilo/loop separado.
//   - Comunicación bidireccional con el editor (inspect nodes, pause, step).
//   - Hot reload de scripts (con VM ARXScript).
//   - Captura de input para que el juego lo reciba.
// ==============================================================================
#pragma once

#include "core/types.hpp"
#include "scene/vox.hpp"

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <thread>
#include <atomic>

namespace arx {

class SceneTree;
class Renderer;
class Window;
class VM;

class EditorPreview {
public:
    enum class State { Stopped, Playing, Paused, Stepping };

    EditorPreview();
    ~EditorPreview();

    bool init(Renderer* renderer, Window* window);
    void shutdown();

    // Start/stop.
    void play();
    void pause();
    void resume();
    void stop();
    void step_one_frame();

    State get_state() const { return state_; }
    bool is_playing() const  { return state_ == State::Playing; }
    bool is_paused() const   { return state_ == State::Paused; }

    // Captura del árbol de voxes para inspección.
    Vox* get_edited_scene() const;
    void  set_edited_scene(Vox* root);

    // Hot reload: re-carga un script sin parar el preview.
    void reload_script(const std::string& path);

    // Inspector runtime: consulta el valor de una propiedad de un node en vivo.
    Variant get_runtime_property(Vox* node, const std::string& property);
    void    set_runtime_property(Vox* node, const std::string& property, Variant value);

    // Stats.
    float get_fps() const { return fps_; }
    int   get_frame_count() const { return frame_count_; }

    // Callbacks.
    void on_state_changed(std::function<void(State)> cb) { on_state_changed_ = std::move(cb); }
    void on_frame(std::function<void()> cb) { on_frame_ = std::move(cb); }

    // Lock/unlock para modificar el SceneTree desde el thread del editor.
    void lock();
    void unlock();

private:
    void run_loop();
    void update_state(State new_state);

    Renderer*    renderer_ = nullptr;
    Window*      window_   = nullptr;
    std::unique_ptr<SceneTree> scene_tree_;
    Vox*        edited_scene_ = nullptr;

    State        state_ = State::Stopped;
    std::atomic<bool> running_{false};
    std::thread  preview_thread_;

    float fps_ = 0.0f;
    int   frame_count_ = 0;
    std::function<void(State)> on_state_changed_;
    std::function<void()>      on_frame_;
};

// LiveEdit — API para editar el SceneTree en runtime desde el editor.
class LiveEdit {
public:
    static LiveEdit& instance();

    void set_preview(EditorPreview* p) { preview_ = p; }

    // Crear/borrar nodes en runtime.
    Vox* instantiate_node(const std::string& class_name, const std::string& name);
    void  remove_node(Vox* node);
    void  reparent_node(Vox* node, Vox* new_parent);

    // Modificar propiedades en runtime.
    void set_property(Vox* node, const std::string& property, Variant value);
    Variant get_property(Vox* node, const std::string& property);

    // Llamar métodos en runtime.
    Variant call_method(Vox* node, const std::string& method,
                         const std::vector<Variant>& args);

    // Watch expressions: el editor monitorea valores que cambian.
    void watch_expression(const std::string& expr, std::function<void(Variant)> cb);
    void clear_watches();
    void update_watches();

private:
    LiveEdit() = default;
    EditorPreview* preview_ = nullptr;
    std::vector<std::pair<std::string, std::function<void(Variant)>>> watches_;
};

} // namespace arx
