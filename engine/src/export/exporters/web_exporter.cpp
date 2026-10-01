// ==============================================================================
// src/export/exporters/web_exporter.cpp
// ==============================================================================
#include "web_exporter.hpp"
#include "core/logging.hpp"

#include <cstdlib>
#include <fstream>

namespace arx {

bool WebExporter::compile_runtime(const fs::path& root,
                                    const ExportPreset& preset,
                                    ExportResult& result) {
    fs::path out_dir = root / "_export_" / preset.platform;
    fs::create_directories(out_dir);

    std::string app = preset.app_name.empty() ? "arx_app" : preset.app_name;

    // CMakeLists para Emscripten.
    std::ofstream cmake(out_dir / "CMakeLists.txt");
    cmake << R"(
cmake_minimum_required(VERSION 3.20)
project()" << app << R"( CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory()" << (fs::current_path() / ".." / "..").string() << R"()

file(GLOB ARX_GENERATED ${CMAKE_CURRENT_SOURCE_DIR}/arx_generated/*.cpp)

add_executable()" << app << R"(
    ${CMAKE_CURRENT_SOURCE_DIR}/main_exported.cpp
    ${ARX_GENERATED}
)
target_link_libraries()" << app << R"( PRIVATE arx_core)
set_target_properties()" << app << R"( PROPERTIES
    OUTPUT_NAME ")" << app << R"("
    SUFFIX ".js"
)
set_target_properties()" << app << R"( PROPERTIES LINK_FLAGS
    "-sALLOW_MEMORY_GROWTH=1 -sUSE_GLFW=3 -sFULL_ES3=1 -sASYNCIFY=1
     -sEXPORTED_RUNTIME_METHODS=['ccall','cwrap'] --preload-file assets@/assets
     --shell-file shell.html")
)";

    // main_exported.cpp (similar a Linux pero con main() adaptado a Emscripten).
    std::ofstream main_cpp(out_dir / "main_exported.cpp");
    main_cpp << R"(
#include "core/types.hpp"
#include "core/logging.hpp"
#include "scene/scene_tree.hpp"
#include "render/renderer.hpp"
#include "platforms/web/os_web.hpp"
#include <emscripten.h>

namespace arx_script { void register_all_classes(); }

int main() {
    static ::arx::OSWeb os;
    if (!os.init()) return 1;

    static auto renderer = ::arx::Renderer::create();
    ::arx::RendererConfig rc; rc.viewport_w = )" << preset.web_canvas_size_w << R"(;
    rc.viewport_h = )" << preset.web_canvas_size_h << R"(;
    renderer->init(rc);

    ::arx::arx_script::register_all_classes();

    static ::arx::SceneTree tree;
    tree.set_window(nullptr);
    tree.set_renderer(renderer.get());

    os.set_main_loop(&tree);
    os.run();
    os.shutdown();
    return 0;
}
)";

    write_html_shell_(out_dir / "shell.html", preset);

    std::string cmd = "emcmake cmake -S \"" + out_dir.string() +
                      "\" -B \"" + (out_dir / "build").string() + "\"";
    result.logs.push_back("Ejecutando: " + cmd);
    if (std::system(cmd.c_str()) != 0) return false;

    cmd = "cmake --build \"" + (out_dir / "build").string() + "\" -j";
    result.logs.push_back("Ejecutando: " + cmd);
    if (std::system(cmd.c_str()) != 0) return false;

    result.source_files_compiled++;
    return true;
}

bool WebExporter::package_output(const fs::path& root,
                                   const ExportPreset& preset,
                                   ExportResult& result) {
    fs::path out_dir = root / "_export_" / preset.platform;
    fs::path build   = out_dir / "build";
    std::string app  = preset.app_name.empty() ? "arx_app" : preset.app_name;

    fs::path js   = build / (app + ".js");
    fs::path wasm = build / (app + ".wasm");
    fs::path data = build / (app + ".data");

    if (!fs::exists(js) || !fs::exists(wasm)) {
        result.error_message = "No se generaron los archivos .js/.wasm";
        return false;
    }

    // Copiar al raíz del out_dir
    fs::copy_file(js,   out_dir / (app + ".js"),   fs::copy_options::overwrite_existing);
    fs::copy_file(wasm, out_dir / (app + ".wasm"), fs::copy_options::overwrite_existing);
    if (fs::exists(data))
        fs::copy_file(data, out_dir / (app + ".data"),
                       fs::copy_options::overwrite_existing);

    // Generar index.html
    std::ofstream html(out_dir / "index.html");
    html << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n";
    html << "<meta charset=\"UTF-8\">\n";
    html << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html << "<title>" << preset.app_name << "</title>\n";
    html << "<style>body{margin:0;background:#000;overflow:hidden}\n";
    html << "canvas{display:block;width:100vw;height:100vh}</style>\n";
    html << "</head>\n<body>\n";
    html << "<canvas id=\"canvas\" oncontextmenu=\"event.preventDefault()\"></canvas>\n";
    html << "<script type=\"text/javascript\">\n";
    html << "  var Module = { canvas: document.getElementById('canvas') };\n";
    html << "</script>\n";
    html << "<script async type=\"text/javascript\" src=\"" << app << ".js\"></script>\n";
    html << "</body>\n</html>\n";

    result.output_path = (out_dir / "index.html").string();
    result.total_bytes = fs::file_size(js) + fs::file_size(wasm);
    return true;
}

void WebExporter::write_html_shell_(const fs::path& path,
                                       const ExportPreset& preset) {
    std::ofstream h(path);
    h << "<!DOCTYPE html>\n<html>\n<head>\n";
    h << "<meta charset=\"utf-8\">\n";
    h << "<title>" << preset.app_name << "</title>\n";
    h << "<style>body{margin:0;background:#0a0a0d;overflow:hidden}\n";
    h << "canvas{width:100vw;height:100vh;display:block}</style>\n";
    h << "</head>\n<body>\n";
    h << "<canvas id=\"canvas\" oncontextmenu=\"event.preventDefault()\"></canvas>\n";
    h << "{{{ SCRIPT }}}\n";   // Emscripten reemplaza esto.
    h << "</body>\n</html>\n";
}

} // namespace arx
