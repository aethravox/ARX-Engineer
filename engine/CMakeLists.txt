# ==============================================================================
# ARX Engine — Root CMakeLists.txt
# Motor 2D/3D de apps y juegos, nativo C++, con editor visual y scripting AOT.
#
# Plataformas soportadas: Windows, Linux, Android (NDK), Web (Emscripten).
# Build system: CMake 3.20+
# ==============================================================================

cmake_minimum_required(VERSION 3.20)

# ---- Proyecto ---------------------------------------------------------------
project(arx_engine
    VERSION 0.0.0
    DESCRIPTION "ARX Engine — 2D/3D apps & games engine with ARXScript AOT"
    LANGUAGES C CXX)

set(ARX_VERSION_MAJOR 0)
set(ARX_VERSION_MINOR 0)
set(ARX_VERSION_PATCH 0)
set(ARX_VERSION_STRING "${ARX_VERSION_MAJOR}.${ARX_VERSION_MINOR}.${ARX_VERSION_PATCH}")
set(ARX_ENGINE_NAME "ARX Engine")

# ---- Opciones ---------------------------------------------------------------
option(ARX_BUILD_EDITOR       "Build the visual editor"                ON)
option(ARX_BUILD_RUNTIME      "Build the headless runtime (export)"    ON)
option(ARX_BUILD_TESTS        "Build unit tests"                       OFF)
option(ARX_BUILD_TOOLS        "Build CLI tools (lint, format, etc.)"   ON)
option(ARX_BUILD_2D           "Include 2D renderer/nodes"              ON)
option(ARX_BUILD_3D           "Include 3D renderer/nodes"              ON)
option(ARX_USE_BULLET         "Use Bullet Physics for 3D"              ON)
option(ARX_USE_FREETYPE       "Use FreeType for font rendering"        ON)
option(ARX_USE_ENET           "Use ENet for networking"                ON)
option(ARX_USE_MBEDTLS        "Use mbedTLS for DTLS/HTTPS"             ON)
option(ARX_USE_MINIMP3        "Use minimp3 for MP3 decoding"           ON)
option(ARX_USE_VORBIS         "Use libvorbis for OGG decoding"         ON)
option(ARX_USE_SVG            "Use nanosvg for SVG rendering"          ON)
option(ARX_USE_RECAST         "Use Recast/Detour for navigation AI"    ON)
option(ARX_USE_OPENGL         "Use OpenGL/GLES renderer"               ON)
option(ARX_USE_VULKAN         "Use Vulkan renderer (experimental)"     OFF)
option(ARX_VERBOSE_LOG        "Verbose logging at compile time"        OFF)

# ---- C++ Standard -----------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

# ---- Posición de binarios ---------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ---- Detección de plataforma ------------------------------------------------
if(EMSCRIPTEN)
    set(ARX_PLATFORM_WEB   ON)
    set(ARX_PLATFORM       "web")
elseif(ANDROID)
    set(ARX_PLATFORM_ANDROID ON)
    set(ARX_PLATFORM         "android")
elseif(WIN32)
    set(ARX_PLATFORM_WINDOWS ON)
    set(ARX_PLATFORM         "windows")
elseif(APPLE)
    set(ARX_PLATFORM_MACOS   ON)
    set(ARX_PLATFORM         "macos")
else()
    set(ARX_PLATFORM_LINUX   ON)
    set(ARX_PLATFORM         "linux")
endif()

message(STATUS "ARX Engine ${ARX_VERSION_STRING}")
message(STATUS "  Platform : ${ARX_PLATFORM}")
message(STATUS "  Compiler : ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  Build    : ${CMAKE_BUILD_TYPE}")

# ---- Flags comunes ----------------------------------------------------------
set(ARX_CORE_COMPILE_FLAGS "")
set(ARX_CORE_LINK_FLAGS    "")

if(MSVC)
    list(APPEND ARX_CORE_COMPILE_FLAGS /W3 /utf-8 /permissive- /Zc:__cplusplus /EHsc /MP)
    list(APPEND ARX_CORE_COMPILE_FLAGS /D_CRT_SECURE_NO_WARNINGS /D_WINSOCK_DEPRECATED_NO_WARNINGS)
else()
    list(APPEND ARX_CORE_COMPILE_FLAGS -Wall -Wextra -Wno-unknown-pragmas -Wno-unused-parameter)
    if(NOT EMSCRIPTEN AND NOT ANDROID)
        list(APPEND ARX_CORE_COMPILE_FLAGS -pthread)
        list(APPEND ARX_CORE_LINK_FLAGS    -pthread)
    endif()
endif()

# ---- Posición del código ----------------------------------------------------
set(ARX_ROOT       ${CMAKE_CURRENT_SOURCE_DIR})
set(ARX_SRC        ${ARX_ROOT}/src)
set(ARX_THIRDPARTY ${ARX_ROOT}/thirdparty)

# ---- Dependencias externas via FetchContent ---------------------------------
include(FetchContent)

# GLFW (windowing + input). En web se usa Emscripten emscripten_* en su lugar.
if(NOT EMSCRIPTEN AND NOT ANDROID)
    FetchContent_Declare(glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.4
        GIT_SHALLOW    TRUE)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glfw)
endif()

# Dear ImGui (editor UI).
if(ARX_BUILD_EDITOR)
    # IMPORTANTE: definir IMGUI_ENABLE_DOCKING e IMGUI_DEFINE_MATH_OPERATORS
    # ANTES de FetchContent_MakeAvailable(imgui) para que:
    # 1. Los headers de imgui declaren las funciones de Docking (DockSpace,
    #    DockBuilder*, ImGuiConfigFlags_DockingEnable, ImGuiCol_DockingPreview,
    #    ImGuiDockNodeFlags_*, ImGuiWindowFlags_NoDocking, etc.)
    # 2. La implementación de esas funciones en imgui.cpp se compile (sino
    #    se declaran pero no se implementan -> errores de linker)
    # Usamos add_compile_definitions en lugar de target_compile_definitions(imgui)
    # porque imgui es un target importado (se crea en su propio CMakeLists en
    # el subdirectorio de FetchContent) y CMake no deja modificarlo desde fuera.
    add_compile_definitions(IMGUI_ENABLE_DOCKING IMGUI_DEFINE_MATH_OPERATORS)

    # IMPORTANTE: Usar la rama 'docking' de ImGui en lugar de un tag específico.
    # El tag 'v1.91.5' del repo master NO tiene las funciones de Docking mergeadas
    # (DockSpace, DockBuilder*, ImGuiConfigFlags_DockingEnable, etc.) - solo
    # aparecen como comentarios "In docking branch". Para tener acceso a las
    # funciones, hay que usar la rama 'docking' oficial de ocornut/imgui que las
    # tiene mergeadas y estables.
    FetchContent_Declare(imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG        docking
        GIT_SHALLOW    TRUE)
    FetchContent_MakeAvailable(imgui)
endif()

# GLM (matemáticas vectoriales header-only).
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
    GIT_SHALLOW    TRUE)
FetchContent_MakeAvailable(glm)

# stb_image (carga de imágenes para texturas, complementa libpng).
FetchContent_Declare(stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        master
    GIT_SHALLOW    TRUE)
FetchContent_MakeAvailable(stb)

# fmt — librería de formateo (usada por logging).
# En Android, fmt no compila con NDK r29, usar stub
if(ARX_PLATFORM_ANDROID)
    add_library(fmt INTERFACE)
    target_include_directories(fmt INTERFACE ${CMAKE_SOURCE_DIR}/thirdparty/fmt-include)
else()
FetchContent_Declare(fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG        10.2.1
    GIT_SHALLOW    TRUE)
set(FMT_DOC OFF CACHE BOOL "" FORCE)
set(FMT_TEST OFF CACHE BOOL "" FORCE)
set(FMT_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(fmt)
endif()

# miniaudio — audio backend header-only (sin dependencias externas).
FetchContent_Declare(miniaudio
    GIT_REPOSITORY https://github.com/mackron/miniaudio.git
    GIT_TAG        master
    GIT_SHALLOW    TRUE)
if(ARX_PLATFORM_ANDROID)
    set(MINIAUDIO_NO_LIBVORBIS ON CACHE BOOL "" FORCE)
    set(MINIAUDIO_NO_OPUS ON CACHE BOOL "" FORCE)
    set(MINIAUDIO_NO_MINIMP3 ON CACHE BOOL "" FORCE)
endif()
FetchContent_MakeAvailable(miniaudio)

# ---- Zen (lenguaje de scripting del motor) ---------------------------------
# Zen compila como libzen.a + ejecutable CLI 'zen'.
# Requiere LLVM 14+ instalado en el sistema.
find_package(LLVM QUIET CONFIG)
if(LLVM_FOUND)
    message(STATUS "ARX: integrando Zen (LLVM ${LLVM_PACKAGE_VERSION})")
    add_subdirectory(zen)
    set(ARX_ZEN_AVAILABLE ON CACHE BOOL "Zen disponible" FORCE)
else()
    message(WARNING "LLVM no encontrado — Zen no se integrará al engine. "
                    "Instalá llvm-dev para habilitar scripting Zen.")
    set(ARX_ZEN_AVAILABLE OFF CACHE BOOL "Zen disponible" FORCE)
endif()

# ---- Submódulos del motor ---------------------------------------------------
add_subdirectory(src)

if(ARX_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(ARX_BUILD_TOOLS)
    add_subdirectory(tools)
endif()

# ---- Info final -------------------------------------------------------------
message(STATUS "")
message(STATUS "ARX Engine configured.")
message(STATUS "  Editor    : ${ARX_BUILD_EDITOR}")
message(STATUS "  Runtime   : ${ARX_BUILD_RUNTIME}")
message(STATUS "  2D/3D     : ${ARX_BUILD_2D}/${ARX_BUILD_3D}")
message(STATUS "  Bullet    : ${ARX_USE_BULLET}")
message(STATUS "")
