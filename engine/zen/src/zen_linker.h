// ==============================================================================
// zen/src/zen_linker.h — Linker integrado para Zen.
//
// Linka .o files a ejecutables para linux/windows/android/web.
// ==============================================================================
#pragma once
#include <string>
#include <vector>

namespace zen {

// Linkar un .o a un ejecutable.
//   obj_path:    path al archivo .o generado por zen --obj
//   output_path: path del ejecutable final
//   platform:    "linux" | "windows" | "android" | "web"
//   libs:        librerías FFI adicionales (ej: ["raylib", "m"])
//   error_msg:   mensaje de error si falla
// Retorna true si el link fue exitoso.
bool link_object(const std::string& obj_path,
                  const std::string& output_path,
                  const std::string& platform,
                  const std::vector<std::string>& libs,
                  std::string& error_msg);

} // namespace zen
