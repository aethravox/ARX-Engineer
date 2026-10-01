// ==============================================================================
// zen/src/zen_api.hpp — API pública de Zen para usar desde el editor.
//
// Permite compilar .zen a ejecutable standalone sin needing el binario `zen` externo.
// El editor linkea zen_static y llama esta API directamente.
// ==============================================================================
#pragma once
#include <string>
#include <vector>

namespace zen {

// Compilar un .zen a ejecutable standalone (compile + link en 1 paso).
//   zen_file:   path al archivo .zen
//   output:     path del ejecutable final
//   platform:   "linux" | "windows" | "android" | "web"
//   error_msg:  mensaje de error si falla
// Retorna true si fue exitoso.
bool compile_and_link(const std::string& zen_file,
                      const std::string& output,
                      const std::string& platform,
                      std::string& error_msg);

// Solo compilar a .o (sin linkar).
bool compile_to_obj(const std::string& zen_file,
                    const std::string& obj_path,
                    const std::string& platform,
                    std::string& error_msg);

// Inicializar LLVM (llamar una vez al arranque).
void initialize();

} // namespace zen
