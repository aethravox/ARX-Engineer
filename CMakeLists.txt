# CMakeLists raiz de ARX
cmake_minimum_required(VERSION 3.20)
project(arx
    VERSION 0.0.1
    LANGUAGES C CXX
    DESCRIPTION "ARX — Motor de apps y juegos nativo C++ con formato .aex firmado"
)

option(ARX_BUILD_ENGINE "Construir ARX Engineer (editor)" ON)
option(ARX_BUILD_CLIENT "Construir ARX Client (runtime)" ON)
option(ARX_BUILD_TOOLS  "Construir CLI tools (sign/pack/verify/keygen)" ON)
option(ARX_BUILD_TESTS  "Construir tests" ON)
option(ARX_BUILD_EXAMPLES "Construir ejemplos" OFF)
option(ARX_USE_LUAU     "Habilitar Luau como scripting sandboxed" ON)
option(ARX_USE_SODIUM   "Usar libsodium para Ed25519 (si no, fallback embebido)" ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Necesario para strdup, etc.
add_compile_definitions(_GNU_SOURCE)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

find_package(Threads REQUIRED)
find_package(ZLIB)
find_package(zstd)

if(ARX_USE_SODIUM)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(SODIUM IMPORTED_TARGET libsodium)
    endif()
    if(NOT SODIUM_FOUND)
        message(STATUS "libsodium no encontrado — se usara ed25519_donna embebida")
    endif()
endif()

add_subdirectory(common)
add_subdirectory(format)

if(ARX_BUILD_ENGINE)
    add_subdirectory(engine)
endif()

if(ARX_BUILD_CLIENT)
    add_subdirectory(client)
endif()

if(ARX_BUILD_TOOLS)
    add_subdirectory(tools/arx_sign)
    add_subdirectory(tools/arx_pack)
    add_subdirectory(tools/arx_verify)
    add_subdirectory(tools/arx_keygen)
endif()

if(ARX_BUILD_TESTS)
    enable_testing()
endif()

if(ARX_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

message(STATUS "")
message(STATUS "=== ARX build configuration ===")
message(STATUS "  Engine : ${ARX_BUILD_ENGINE}")
message(STATUS "  Client : ${ARX_BUILD_CLIENT}")
message(STATUS "  Tools  : ${ARX_BUILD_TOOLS}")
message(STATUS "  Tests  : ${ARX_BUILD_TESTS}")
message(STATUS "  Luau   : ${ARX_USE_LUAU}")
message(STATUS "  Sodium : ${SODIUM_FOUND}")
message(STATUS "  Build  : ${CMAKE_BUILD_TYPE}")
message(STATUS "================================")
