// ==============================================================================
// src/export/exporters/windows_exporter.cpp
// ==============================================================================
#include "windows_exporter.hpp"
#include "core/logging.hpp"

#include <cstdlib>
#include <fstream>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#endif

namespace arx {

bool WindowsExporter::can_export() const {
#ifdef _WIN32
    return !find_cl_exe_().empty();
#else
    // En Linux/Mac se podría usar MinGW cross-compiler.
    return std::system("which x86_64-w64-mingw32-g++ > /dev/null 2>&1") == 0;
#endif
}

bool WindowsExporter::compile_runtime(const fs::path& root,
                                        const ExportPreset& preset,
                                        ExportResult& result) {
    fs::path out_dir = root / "_export_" / preset.platform;
    fs::path bin_dir = out_dir / "bin";
    fs::create_directories(bin_dir);

    // 1. Generar CMakeLists para el proyecto exportado.
    std::string app_name = preset.app_name.empty() ? "arx_app" : preset.app_name;
    std::ofstream cmake(out_dir / "CMakeLists.txt");
    cmake << R"(
cmake_minimum_required(VERSION 3.20)
project()" << app_name << R"( CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Núcleo del motor
add_subdirectory()" << (fs::current_path() / ".." / "..").string() << R"()

# Fuentes ARX transpiladas
file(GLOB ARX_GENERATED ${CMAKE_CURRENT_SOURCE_DIR}/arx_generated/*.cpp)

# Fuentes del runtime
add_executable()" << app_name << R"(
    ${CMAKE_CURRENT_SOURCE_DIR}/main_exported.cpp
    ${ARX_GENERATED}
)
target_link_libraries()" << app_name << R"( PRIVATE arx_core)
set_target_properties()" << app_name << R"( PROPERTIES WIN32_EXECUTABLE ON)
)";

    // 2. main_exported.cpp que arranca el runtime + carga main.scene.
    std::ofstream main_cpp(out_dir / "main_exported.cpp");
    main_cpp << R"(
#include "core/types.hpp"
#include "core/logging.hpp"
#include "os/os.hpp"
#include "scene/scene_tree.hpp"
#include "render/renderer.hpp"

// Auto-registro de clases ARX
namespace arx_script {
    void register_all_classes();
}

int main(int argc, char** argv) {
    auto os = std::unique_ptr<::arx::OS>(::arx::OS::create());
    if (!os->init()) return 1;

    ::arx::WindowCreateInfo wci;
    wci.title = ")" << app_name << R"(";
    wci.width = 1280; wci.height = 720;
    auto win = os->create_window(wci);

    auto renderer = ::arx::Renderer::create();
    ::arx::RendererConfig rc;
    rc.viewport_w = 1280; rc.viewport_h = 720;
    renderer->init(rc);

    ::arx::arx_script::register_all_classes();

    ::arx::SceneTree tree;
    tree.init(win.get(), renderer.get());
    // TODO: cargar scenes/main.scene y setearlo como root.

    os->set_main_loop(&tree);
    os->run();
    os->shutdown();
    return 0;
}
)";

    // 3. Llamar a CMake para configurar + compilar.
    std::string cmd = "cmake -S \"" + out_dir.string() +
                      "\" -B \"" + (out_dir / "build").string() +
                      "\" -G \"Visual Studio 17 2022\" -A x64";
    result.logs.push_back("Ejecutando: " + cmd);
    int rc = std::system(cmd.c_str());
    if (rc != 0) {
        result.error_message = "CMake configure fallido";
        return false;
    }

    cmd = "cmake --build \"" + (out_dir / "build").string() +
          "\" --config Release";
    result.logs.push_back("Ejecutando: " + cmd);
    rc = std::system(cmd.c_str());
    if (rc != 0) {
        result.error_message = "CMake build fallido";
        return false;
    }

    result.source_files_compiled++;
    return true;
}

bool WindowsExporter::package_output(const fs::path& root,
                                       const ExportPreset& preset,
                                       ExportResult& result) {
    fs::path out_dir = root / "_export_" / preset.platform;
    fs::path exe = out_dir / "build" / "Release" / (preset.app_name + ".exe");
    if (!fs::exists(exe)) {
        // Fallback: buscar en cualquier subdirectorio.
        for (auto& e : fs::recursive_directory_iterator(out_dir / "build")) {
            if (e.path().extension() == ".exe") { exe = e.path(); break; }
        }
    }
    if (!fs::exists(exe)) {
        result.error_message = "No se encontró el .exe generado";
        return false;
    }

    // Copiar el exe a la raíz del directorio de export.
    fs::path final = out_dir / (preset.app_name + ".exe");
    fs::copy_file(exe, final, fs::copy_options::overwrite_existing);

    // Copiar assets.
    if (preset.embed_assets && fs::exists(root / "assets")) {
        copy_directory_recursive(root / "assets", out_dir / "assets");
    }

    // Copiar icon (TODO: embeber en el .exe via .rc).
    result.output_path = final.string();
    result.total_bytes = fs::file_size(final);
    return true;
}

std::string WindowsExporter::find_msvc_() {
    // TODO: usar vswhere.
    return "";
}

std::string WindowsExporter::find_cl_exe_() {
#ifdef _WIN32
    // TODO: detectar via vswhere.
    return "cl.exe";
#else
    return "";
#endif
}

} // namespace arx
