// ==============================================================================
// src/export/exporters/linux_exporter.cpp
// ==============================================================================
#include "linux_exporter.hpp"
#include "core/logging.hpp"

#include <cstdlib>
#include <fstream>

namespace arx {

bool LinuxExporter::compile_runtime(const fs::path& root,
                                      const ExportPreset& preset,
                                      ExportResult& result) {
    fs::path out_dir = root / "_export_" / preset.platform;
    fs::create_directories(out_dir);

    // Generar CMakeLists.
    std::ofstream cmake(out_dir / "CMakeLists.txt");
    cmake << R"(
cmake_minimum_required(VERSION 3.20)
project()" << preset.app_name << R"( CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory()" << (fs::current_path() / ".." / "..").string() << R"()

file(GLOB ARX_GENERATED ${CMAKE_CURRENT_SOURCE_DIR}/arx_generated/*.cpp)

add_executable()" << preset.app_name << R"(
    ${CMAKE_CURRENT_SOURCE_DIR}/main_exported.cpp
    ${ARX_GENERATED}
)
target_link_libraries()" << preset.app_name << R"( PRIVATE arx_core)
)";

    // main_exported.cpp similar al de Windows.
    std::ofstream main_cpp(out_dir / "main_exported.cpp");
    main_cpp << R"(
#include "core/types.hpp"
#include "core/logging.hpp"
#include "os/os.hpp"
#include "scene/scene_tree.hpp"
#include "render/renderer.hpp"

namespace arx_script { void register_all_classes(); }

int main(int argc, char** argv) {
    auto os = std::unique_ptr<::arx::OS>(::arx::OS::create());
    if (!os->init()) return 1;

    ::arx::WindowCreateInfo wci;
    wci.title = ")" << preset.app_name << R"(";
    wci.width = 1280; wci.height = 720;
    auto win = os->create_window(wci);

    auto renderer = ::arx::Renderer::create();
    ::arx::RendererConfig rc; rc.viewport_w = 1280; rc.viewport_h = 720;
    renderer->init(rc);

    ::arx::arx_script::register_all_classes();

    ::arx::SceneTree tree;
    tree.init(win.get(), renderer.get());

    os->set_main_loop(&tree);
    os->run();
    os->shutdown();
    return 0;
}
)";

    std::string cmd = "cmake -S \"" + out_dir.string() + "\" -B \"" +
                      (out_dir / "build").string() + "\" -DCMAKE_BUILD_TYPE=Release";
    result.logs.push_back("Ejecutando: " + cmd);
    if (std::system(cmd.c_str()) != 0) return false;

    cmd = "cmake --build \"" + (out_dir / "build").string() + "\" --config Release -j";
    result.logs.push_back("Ejecutando: " + cmd);
    if (std::system(cmd.c_str()) != 0) return false;

    result.source_files_compiled++;
    return true;
}

bool LinuxExporter::package_output(const fs::path& root,
                                     const ExportPreset& preset,
                                     ExportResult& result) {
    fs::path out_dir = root / "_export_" / preset.platform;
    fs::path bin = out_dir / "build" / preset.app_name;
    if (!fs::exists(bin)) {
        result.error_message = "No se encontró el binario generado";
        return false;
    }

    fs::path final = out_dir / preset.app_name;
    fs::copy_file(bin, final, fs::copy_options::overwrite_existing);
    fs::permissions(final,
                    fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec,
                    fs::perm_options::add);

    if (preset.embed_assets && fs::exists(root / "assets"))
        copy_directory_recursive(root / "assets", out_dir / "assets");

    result.output_path = final.string();
    result.total_bytes = fs::file_size(final);
    return true;
}

} // namespace arx
