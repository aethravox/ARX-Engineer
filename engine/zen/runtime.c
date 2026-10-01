// ============================================================
// Zen Programming Language - Runtime en C
// Funciones auxiliares que el LLVM IR llama
// ============================================================

#include <string.h>
#include <stdlib.h>

// Concatenacion de strings: "hola" + " mundo"
// Llamada desde LLVM IR cuando se usa + con dos textos
char* zen_concat(const char* a, const char* b) {
    size_t la = strlen(a);
    size_t lb = strlen(b);
    char* result = (char*)malloc(la + lb + 1);
    if (!result) return (char*)"";
    memcpy(result, a, la);
    memcpy(result + la, b, lb + 1);
    return result;
}
