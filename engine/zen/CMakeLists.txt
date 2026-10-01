# ============================================================
# Zen Programming Language - CMake Build Configuration
# Requiere: LLVM 14+, CMake 3.16+, C++17
# Compilacion nativa sin Clang
# ============================================================

cmake_minimum_required(VERSION 3.16)
project(Zen LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Buscar LLVM
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Zen: LLVM encontrado: ${LLVM_PACKAGE_VERSION}")
message(STATUS "Zen: LLVM include: ${LLVM_INCLUDE_DIRS}")

# Incluir headers de LLVM
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Obtener librerias LLVM necesarias
# === STATIC LINK: embeber LLVM en el binario (1 solo archivo, sin .so externas) ===
# Usar --link-static para obtener los .a en vez de -lLLVM-14 (.so)
execute_process(
    COMMAND llvm-config-14 --link-static --libfiles
    OUTPUT_VARIABLE LLVM_LIBFILES
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND llvm-config-14 --link-static --system-libs
    OUTPUT_VARIABLE LLVM_SYSLIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND llvm-config-14 --ldflags
    OUTPUT_VARIABLE LLVM_LDFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
# Convertir a listas (manejar espacios múltiples y newlines)
string(REPLACE " " ";" llvm_libs "${LLVM_LIBFILES}")
# Remover elementos vacíos
list(REMOVE_ITEM llvm_libs "")
# Syslibs y ldflags se pasan como strings al linker
set(LLVM_EXTRA_SYSLIBS "${LLVM_SYSLIBS}")
set(LLVM_EXTRA_LDFLAGS "${LLVM_LDFLAGS}")
message(STATUS "Zen: LLVM static libs count: ${llvm_libs}")
message(STATUS "Zen: LLVM syslibs: ${LLVM_EXTRA_SYSLIBS}")

# ============================================================
# libzen — librería estática del compilador Zen (reutilizable desde el engine)
# ============================================================
add_library(zen_static STATIC
    src/lexer.cpp
    src/parser.cpp
    src/codegen.cpp
    src/zen_vm.cpp
    src/zen_linker.cpp
    src/zen_runtimes.cpp
    src/zen_api.cpp
)

target_include_directories(zen_static PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${LLVM_INCLUDE_DIRS}
)

target_compile_definitions(zen_static PUBLIC ${LLVM_DEFINITIONS})
target_link_libraries(zen_static PUBLIC ${llvm_libs} ${LLVM_EXTRA_SYSLIBS} ${LLVM_EXTRA_LDFLAGS} dl m pthread)

# Renombrar el output a 'libzen.a' para que sea más claro
set_target_properties(zen_static PROPERTIES OUTPUT_NAME "zen")

# ============================================================
# zen — ejecutable CLI (CLI wrapper sobre libzen_static)
# ============================================================
add_executable(zen_bin src/main.cpp)
target_link_libraries(zen_bin PRIVATE zen_static)
set_target_properties(zen_bin PROPERTIES OUTPUT_NAME "zen")

# Instalar
install(TARGETS zen_static zen_bin DESTINATION bin)
