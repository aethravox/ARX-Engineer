// ==============================================================================
// src/runtime_main.cpp — Entry point del RUNTIME (modo export, sin editor).
// Carga un proyecto ARX y lo ejecuta. Se usa cuando se quiere probar un
// proyecto sin pasar por el editor, o como base para el export.
// ==============================================================================
#include "core/logging.hpp"
#include "core/object.hpp"
#include "os/os.hpp"
#include "render/renderer.hpp"
#include "scene/scene_tree.hpp"
#include "physics/physics_server.hpp"
#include "modules/register_module_types.hpp"

#include <memory>
#include <iostream>
#include <filesystem>

int main(int argc, char** argv) {
    using namespace arx;

    Logger::instance().set_min_level(LogLevel::Debug);
    Logger::instance().set_log_file_path("/tmp/arx_runtime.log");
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    ARX_LOG_INFO("=== ARX Engine {} — Runtime ===", ARX_VERSION);

    std::string project_path = ".";
    if (argc > 1) project_path = argv[1];

    if (!std::filesystem::exists(project_path + "/project.arx")) {
        ARX_LOG_ERROR("No se encontró project.arx en: {}", project_path);
        return 1;
    }
    ARX_LOG_INFO("Proyecto encontrado: {}/project.arx", project_path);

    auto os = std::unique_ptr<OS>(OS::create());
    if (!os) {
        ARX_LOG_FATAL("OS::create() retornó null (¿plategoria no soportada?)");
        return 1;
    }
    if (!os->init()) {
        ARX_LOG_FATAL("OS::init() falló (¿glfwInit falló?)");
        return 1;
    }
    ARX_LOG_INFO("OS inicializado OK");

    WindowCreateInfo wci;
    wci.title    = "ARX Runtime";
    wci.width    = 1280;
    wci.height   = 720;
    wci.gl_major = 2;
    wci.gl_minor = 1;
    wci.gl_compat = true;
    auto window = os->create_window(wci);
    if (!window) {
        ARX_LOG_FATAL("No se pudo crear la ventana del runtime");
        return 1;
    }
    window->set_vsync(true);
    ARX_LOG_INFO("Ventana creada OK");

    auto renderer = Renderer::create(Renderer::Backend::OpenGL);
    RendererConfig rc;
    rc.viewport_w = wci.width;
    rc.viewport_h = wci.height;
    if (!renderer->init(rc)) {
        ARX_LOG_FATAL("No se pudo inicializar el renderer");
        return 1;
    }
    ARX_LOG_INFO("Renderer inicializado OK");

    register_module_types();

    SceneTree tree;
    tree.init(window.get(), renderer.get());

    // ===== TEST DE FÍSICA: crear un cuerpo que cae y un piso =====
    // Esto prueba que Bullet está conectado y funcionando.
    auto* ps = tree.get_physics_server();
    if (ps) {
        ARX_LOG_INFO("=== TEST DE FÍSICA ===");
        // Crear un space (mundo)
        auto space = ps->space_create();
        ps->space_set_gravity(space, Vector3{0.0f, -9.8f, 0.0f});
        ps->space_set_active(space, true);

        // Crear un piso estático (box grande)
        auto floor_shape = ps->shape_create(ShapeType::Box, Vector3{10.0f, 0.5f, 10.0f});
        Transform3D floor_tf;
        floor_tf.origin = Vector3{0.0f, -2.0f, 0.0f};
        auto floor_body = ps->body_create(BodyType::Static, space, floor_shape, floor_tf);
        ARX_LOG_INFO("Piso creado: pos=({}, {}, {})", floor_tf.origin.x, floor_tf.origin.y, floor_tf.origin.z);

        // Crear una caja dinámica que cae
        auto box_shape = ps->shape_create(ShapeType::Box, Vector3{0.5f, 0.5f, 0.5f});
        Transform3D box_tf;
        box_tf.origin = Vector3{0.0f, 5.0f, 0.0f};  // 5 metros de altura
        auto box_body = ps->body_create(BodyType::Dynamic, space, box_shape, box_tf);
        ps->body_set_mass(box_body, 1.0f);
        ARX_LOG_INFO("Caja creada: pos=({}, {}, {})", box_tf.origin.x, box_tf.origin.y, box_tf.origin.z);

        // Simular 60 frames (1 segundo) y loggear la posición de la caja
        ARX_LOG_INFO("Simulando 60 frames...");
        for (int i = 0; i < 60; i++) {
            ps->step(1.0f / 60.0f);
            auto t = ps->body_get_transform(box_body);
            if (i % 10 == 0) {
                ARX_LOG_INFO("  Frame {}: caja Y = {:.3f}", i, t.origin.y);
            }
        }
        auto final_tf = ps->body_get_transform(box_body);
        ARX_LOG_INFO("Posición final de la caja: Y = {:.3f}", final_tf.origin.y);
        if (final_tf.origin.y < 1.0f) {
            ARX_LOG_INFO("✓ FÍSICA FUNCIONANDO: la caja cayó y se detuvo en el piso!");
        } else {
            ARX_LOG_WARN("⚠ La caja no llegó al piso (Y={:.3f})", final_tf.origin.y);
        }
        ARX_LOG_INFO("=== FIN TEST DE FÍSICA ===");
    }

    // TODO: cargar scenes/main.scene y setearlo como root del SceneTree.

    os->set_main_loop(&tree);
    os->run();

    tree.finish();
    renderer->shutdown();
    os->shutdown();
    return 0;
}
