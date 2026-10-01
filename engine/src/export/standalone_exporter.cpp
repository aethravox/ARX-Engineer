// ==============================================================================
// engine/src/export/standalone_exporter.cpp
//
// Implementación del exporter standalone (Modo 2: sin motor, con FFI).
// ==============================================================================

#include "standalone_exporter.hpp"
#include "core/logging.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>

namespace arx {

namespace fs = std::filesystem;

// Helper: ejecutar un comando del sistema y capturar exit code
static int run_cmd(const std::string& cmd, std::string& output) {
    ARX_LOG_INFO("[export] $ {}", cmd);
    // Redirigir stderr a stdout para capturar todo
    std::string full_cmd = cmd + " 2>&1";
    FILE* pipe = popen(full_cmd.c_str(), "r");
    if (!pipe) return -1;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        output += buffer;
    }
    int rc = pclose(pipe);
    return WEXITSTATUS(rc);
}

bool StandaloneExporter::can_export() const {
#ifdef ARX_ZEN_ENABLED
    return true;
#else
    return false;
#endif
}

std::string StandaloneExporter::llvm_target_triple() const {
    if (platform_ == "linux")   return "x86_64-unknown-linux-gnu";
    if (platform_ == "windows") return "x86_64-pc-windows-gnu";  // MinGW
    if (platform_ == "android") return "aarch64-linux-android";
    if (platform_ == "web")     return "wasm32-unknown-emscripten";
    return "x86_64-unknown-linux-gnu";
}

std::string StandaloneExporter::executable_extension() const {
    if (platform_ == "linux")   return "";      // ELF sin extensión
    if (platform_ == "windows") return ".exe";
    if (platform_ == "android") return ".apk";  // simplificado
    if (platform_ == "web")     return ".html";
    return "";
}

bool StandaloneExporter::compile_runtime(const fs::path& root,
                                           const ExportPreset& preset,
                                           ExportResult& result) {
    ARX_LOG_INFO("[standalone] Compilando para '{}' (target LLVM: {})",
                 platform_, llvm_target_triple());

#ifndef ARX_ZEN_ENABLED
    result.error_message = "Zen no está disponible en este build del editor. "
                           "Recompilá con LLVM instalado.";
    return false;
#else
    // 1. Encontrar todos los .zen del proyecto
    auto arx_files = find_arx_files(root);
    if (arx_files.empty()) {
        result.error_message = "No se encontraron archivos .zen en el proyecto";
        return false;
    }
    result.logs.push_back("Archivos .zen encontrados: " + std::to_string(arx_files.size()));

    // 2. Crear directorio de build temporal
    fs::path build_dir = root / "_export_" / platform_ / "obj";
    fs::create_directories(build_dir);

    // 3. Compilar cada .zen con el CLI `zen` (más robusto que el bridge C++)
    // Buscar el binario `zen` en PATH o en ubicaciones conocidas
    std::string zen_bin;
    {
        std::string out;
        int rc = run_cmd("which zen 2>/dev/null || echo /tmp/zen_build/zen", out);
        // Quitar newline
        zen_bin = out;
        while (!zen_bin.empty() && (zen_bin.back() == '\n' || zen_bin.back() == '\r'))
            zen_bin.pop_back();
        if (rc != 0 || zen_bin.empty() || !fs::exists(zen_bin)) {
            result.error_message = "No se encontró el binario 'zen'. Compilá Zen primero.";
            return false;
        }
        ARX_LOG_INFO("[standalone] Usando zen: {}", zen_bin);
    }

    std::vector<std::string> obj_paths;
    std::vector<std::string> all_ffi_libs;

    for (const auto& zen_file : arx_files) {
        ARX_LOG_INFO("[standalone] Compilando {}", zen_file.string());

        // Generar .o en el directorio de build
        fs::path obj_path = build_dir / (zen_file.stem().string() + ".o");

        // El CLI `zen` genera el .o en el directorio actual con el mismo nombre
        // Cambiar al directorio de build y llamar zen --obj --ffunction-sections --fdata-sections
        // === Cross-compile en 2 pasos ===
        // Paso 1: zen file.zen → genera .ll (LLVM IR, portable)
        std::string cmd1 = "cd \"" + build_dir.string() + "\" && " + zen_bin + " \"" +
                           zen_file.string() + "\" 2>&1";
        std::string compile_out1;
        int rc1 = run_cmd(cmd1, compile_out1);
        fs::path generated_ll = build_dir / (zen_file.stem().string() + ".ll");
        if (rc1 != 0 || !fs::exists(generated_ll)) {
            result.error_message = "Error generando .ll para " + zen_file.string() + ":\n" + compile_out1;
            return false;
        }

        // Paso 2: clang --target=<triple> -c file.ll → genera .o para la plataforma
        std::string clang_bin = "clang-14";
        // Verificar si clang-14 existe, sino usar clang
        if (!fs::exists("/usr/bin/clang-14")) clang_bin = "clang";

        std::string target_triple = llvm_target_triple();
        // Para Android, usar el NDK clang
        std::string compile_cmd;
        if (platform_ == "android") {
            const char* ndk = std::getenv("ANDROID_NDK_HOME");
            if (!ndk || !*ndk) ndk = "/home/aethravox/Escritorio/p/mis_cosas/Android/linux/android-ndk-r29";
            std::string ndk_clang = std::string(ndk) + "/toolchains/llvm/prebuilt/linux-x86_64/bin/clang";
            compile_cmd = ndk_clang + " -c \"" + generated_ll.string() + "\" -o \"" +
                         (build_dir / (zen_file.stem().string() + ".o")).string() + "\" --target=" +
                         target_triple + " -O2 -ffunction-sections -fdata-sections 2>&1";
        } else {
            compile_cmd = clang_bin + " -c \"" + generated_ll.string() + "\" -o \"" +
                         (build_dir / (zen_file.stem().string() + ".o")).string() + "\" --target=" +
                         target_triple + " -O2 -ffunction-sections -fdata-sections 2>&1";
        }
        std::string compile_out2;
        int rc2 = run_cmd(compile_cmd, compile_out2);

        fs::path generated_obj = build_dir / (zen_file.stem().string() + ".o");
        if (rc2 != 0 || !fs::exists(generated_obj)) {
            result.error_message = "Error compilando .o para " + zen_file.string() + " (target=" +
                                   target_triple + "):\n" + compile_out2;
            return false;
        }

        obj_paths.push_back(generated_obj.string());
        result.arx_files_transpiled++;
        result.logs.push_back("  ✓ " + zen_file.string() + " → " + generated_obj.string());
    }

    // Guardar info para package_output
    result.output_path = build_dir.string();  // temporal
    result.total_bytes = 0;
    for (const auto& obj : obj_paths) {
        if (fs::exists(obj)) result.total_bytes += fs::file_size(obj);
    }
    // Pasar las obj_paths y ffi_libs via output_path (hack sencillo)
    // En una implementación completa usaríamos un campo extra en ExportResult
    std::ostringstream oss;
    oss << build_dir.string() << "\n";
    for (const auto& o : obj_paths) oss << "OBJ:" << o << "\n";
    for (const auto& l : all_ffi_libs) oss << "LIB:" << l << "\n";
    result.error_message = oss.str();  // abuso del campo para pasar datos

    return true;
#endif
}

bool StandaloneExporter::link_objects(const std::vector<std::string>& obj_paths,
                                        const std::vector<std::string>& ffi_libs,
                                        const fs::path& output_path,
                                        ExportResult& result) {
    // Construir comando de link
    std::ostringstream cmd;

    // Compilador según plataforma target
    if (platform_ == "linux") {
        cmd << "cc";
    } else if (platform_ == "windows") {
        cmd << "x86_64-w64-mingw32-gcc";
    } else if (platform_ == "android") {
        // Usar NDK clang desde $ANDROID_NDK_HOME o path conocido
        const char* ndk = std::getenv("ANDROID_NDK_HOME");
        if (!ndk || !*ndk) ndk = "/home/aethravox/Escritorio/p/mis_cosas/Android/linux/android-ndk-r29";
        std::string clang = std::string(ndk) + "/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android24-clang";
        cmd << clang;
    } else if (platform_ == "web") {
        cmd << "emcc";
    } else {
        cmd << "cc";
    }

    // Objects
    for (const auto& obj : obj_paths) {
        cmd << " " << obj;
    }

    // Libs FFI (raylib, etc.)
    for (const auto& lib : ffi_libs) {
        if (lib == "raylib") {
            // raylib en Linux necesita OpenGL, X11, etc.
            cmd << " -lraylib -lGL -lX11 -lpthread -ldl -lrt -lm";
        } else {
            cmd << " -l" << lib;
        }
    }

    // Output
    cmd << " -o " << output_path.string();

    // Optimización (si preset.optimize)
    if (true) cmd << " -O2";

    // === Dead code elimination (item 67) ===
    // --gc-sections elimina funciones/datos no referenciados → binario más pequeño
    // -s strip simbolos para reducir tamaño del binario final
    // Para web (emcc), --gc-sections es default pero -s tiene otro significado
    if (platform_ == "web") {
        cmd << " -Wl,--gc-sections";
        // emcc usa -O3 para optimización agresiva + --closure para JS
        cmd << " --closure 1";
    } else {
        cmd << " -Wl,--gc-sections -Wl,--strip-all";
    }

    // Para Windows: -static para que no dependan de DLLs de MinGW
    if (platform_ == "windows") {
        cmd << " -static";
    }
    // Para Android: -pie (obligatorio para API 21+), -lm -llog
    if (platform_ == "android") {
        cmd << " -pie -lm -llog";
    }

    // Ejecutar link
    std::string link_output;
    int rc = run_cmd(cmd.str(), link_output);
    if (rc != 0) {
        result.error_message = "Link fallido (código " + std::to_string(rc) + "):\n" + link_output;
        return false;
    }
    result.logs.push_back("Link OK: " + output_path.string());
    return true;
}

bool StandaloneExporter::package_output(const fs::path& root,
                                          const ExportPreset& preset,
                                          ExportResult& result) {
    // Parsear obj_paths y ffi_libs de result.error_message (hack de compile_runtime)
    std::istringstream iss(result.error_message);
    std::string line;
    fs::path build_dir;
    std::vector<std::string> obj_paths;
    std::vector<std::string> ffi_libs;
    bool first_line = true;
    while (std::getline(iss, line)) {
        if (first_line) {
            build_dir = line;
            first_line = false;
            continue;
        }
        if (line.rfind("OBJ:", 0) == 0) obj_paths.push_back(line.substr(4));
        else if (line.rfind("LIB:", 0) == 0) ffi_libs.push_back(line.substr(4));
    }
    result.error_message.clear();

    // Path de salida
    std::string exe_name = preset.app_name.empty() ? "game" : preset.app_name;
    // Limpiar nombre (sin espacios ni caracteres raros)
    std::string clean_name;
    for (char c : exe_name) {
        if (std::isalnum(c) || c == '_' || c == '-') clean_name += c;
    }
    if (clean_name.empty()) clean_name = "game";

    fs::path output_path = root / "_export_" / platform_ / (clean_name + executable_extension());
    fs::create_directories(output_path.parent_path());

    // Linkar
    if (!link_objects(obj_paths, ffi_libs, output_path, result)) {
        return false;
    }

    // En Linux, dar permiso de ejecución
    if (platform_ == "linux") {
        fs::permissions(output_path,
                        fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec,
                        fs::perm_options::add);
    }

    // Stats finales
    if (fs::exists(output_path)) {
        result.total_bytes = fs::file_size(output_path);
    }
    result.output_path = output_path.string();
    result.success = true;

    ARX_LOG_INFO("[standalone] Export OK: {} ({} bytes)",
                 output_path.string(), result.total_bytes);
    return true;
}

} // namespace arx
