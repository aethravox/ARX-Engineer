// ==============================================================================
// zen/src/zen_linker.cpp — Linker integrado para Zen usando lld + runtimes embebidos.
//
// Usa lld-14 para linkar y los runtimes embebidos (crt+libs) de zen_runtimes.cpp.
// Esto hace que el binario zen sea 100% standalone — no necesita MinGW ni libc
// del sistema instalados.
//
// ==============================================================================
#include "zen_linker.h"
#include "zen_runtimes.hpp"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <sstream>

namespace fs = std::filesystem;

namespace zen {

// Helper: ejecutar comando y capturar exit code
static int run_cmd(const std::string& cmd, std::string& output) {
    std::string full = cmd + " 2>&1";
    FILE* pipe = popen(full.c_str(), "r");
    if (!pipe) return -1;
    char buf[256];
    while (fgets(buf, sizeof(buf), pipe)) output += buf;
    int rc = pclose(pipe);
    return WEXITSTATUS(rc);
}

// Helper: encontrar lld-14
static std::string find_lld() {
    if (fs::exists("/usr/bin/ld.lld-14")) return "/usr/bin/ld.lld-14";
    if (fs::exists("/usr/bin/ld.lld")) return "/usr/bin/ld.lld";
    return "";
}

// Helper: encontrar NDK clang (para Android, todavía necesario)
static std::string find_ndk_clang() {
    const char* ndk = std::getenv("ANDROID_NDK_HOME");
    if (!ndk || !*ndk) {
        ndk = "/home/aethravox/Escritorio/p/mis_cosas/Android/linux/android-ndk-r29";
    }
    std::string clang = std::string(ndk) + "/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android24-clang";
    if (fs::exists(clang)) return clang;
    return "";
}

bool link_object(const std::string& obj_path,
                  const std::string& output_path,
                  const std::string& platform,
                  const std::vector<std::string>& libs,
                  std::string& error_msg) {
    std::ostringstream cmd;
    std::string linker_output;

    // Extraer runtimes embebidos a /tmp/arx_runtime/
    std::string rt_dir = extract_runtimes();

    if (platform == "linux") {
        // === Linux: usar ld.lld-14 + crt embebidos + libc dinámica del sistema ===
        // Los crt (crt1.o, crti.o, crtn.o) van embebidos.
        // libc/libm/libdl se usan dinámicas (casi todo Linux las tiene).
        std::string lld = find_lld();
        if (lld.empty()) {
            error_msg = "ld.lld-14 no encontrado. Instalá: sudo apt install lld-14";
            return false;
        }

        std::string linux_dir = rt_dir + "/linux";

        cmd << lld << " -flavor gnu"
            << " " << linux_dir << "/crt1.o"
            << " " << linux_dir << "/crti.o"
            << " " << obj_path
            << " " << linux_dir << "/crtn.o"
            << " -L/lib/x86_64-linux-gnu -L/usr/lib/x86_64-linux-gnu"
            << " -lc -lm -ldl";  // dinámicas del sistema

        // Libs FFI del usuario
        for (const auto& lib : libs) {
            cmd << " -l" << lib;
        }

        cmd << " -o " << output_path;
        cmd << " -dynamic-linker /lib64/ld-linux-x86-64.so.2";
        cmd << " --gc-sections --strip-all";

    } else if (platform == "windows") {
        // === Windows: usar ld.lld-14 -flavor gnu + runtimes MinGW embebidos ===
        std::string lld = find_lld();
        if (lld.empty()) {
            error_msg = "ld.lld-14 no encontrado. Instalá: sudo apt install lld-14";
            return false;
        }

        std::string win_dir = rt_dir + "/windows";

        cmd << lld << " -flavor gnu"
            << " -m i386pep"  // Windows PE 64-bit
            << " " << win_dir << "/crt2.o"
            << " " << obj_path
            << " -L" << win_dir
            << " -lmsvcrt -lmingw32 -lmingwex -lmoldname -lgcc -lkernel32 -luser32";

        // Libs FFI del usuario
        for (const auto& lib : libs) {
            cmd << " -l" << lib;
        }

        cmd << " -o " << output_path;
        cmd << " --gc-sections --strip-all";

    } else if (platform == "android") {
        // === Android: usar NDK clang (que invoca lld internamente) ===
        std::string clang = find_ndk_clang();
        if (clang.empty()) {
            error_msg = "NDK no encontrado. Setea ANDROID_NDK_HOME";
            return false;
        }

        cmd << clang
            << " " << obj_path
            << " -o " << output_path
            << " -pie -lm -llog -Wl,--gc-sections -Wl,--strip-all";

        // Libs FFI del usuario
        for (const auto& lib : libs) {
            cmd << " -l" << lib;
        }

    } else if (platform == "web") {
        // === Web: usar emcc (Emscripten) ===
        cmd << "source /home/aethravox/emsdk/emsdk_env.sh 2>/dev/null && emcc"
            << " " << obj_path
            << " -o " << output_path
            << " --closure 1 -s WASM=1 -O2";

    } else {
        error_msg = "Plataforma no soportada: " + platform;
        return false;
    }

    // Ejecutar
    int rc = run_cmd(cmd.str(), linker_output);
    if (rc != 0) {
        error_msg = "Link fallido (código " + std::to_string(rc) + "):\n" + linker_output;
        return false;
    }

    return true;
}

} // namespace zen
