# =============================================================
# ARX ENGINE - CLIENTE - Android CMakeLists.txt
# Requiere: Android NDK (arm64-v8a, API 21+)
# Librerias: raylib, assimp, imgui, Jolt, glm, zlib
# Todas desde third_Party
# =============================================================
cmake_minimum_required(VERSION 3.15)
project(ARX_Cliente CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Rutas
set(THIRD_PARTY "${CMAKE_SOURCE_DIR}/../../../third_Party")
set(COMMON_DIR  "${CMAKE_SOURCE_DIR}/../../../common")
set(MOTOR_PATH  "${CMAKE_SOURCE_DIR}/../../../Motor/platform/android")

# Directorios de inclusion
# Nota: raylib, imgui SIN prefijo; Jolt/assimp/glm CON prefijo -> apuntan a Include/
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/scripting
    ${THIRD_PARTY}/Include
    ${THIRD_PARTY}/Include/raylib
    ${THIRD_PARTY}/Include/imgui
    ${COMMON_DIR}
    ${MOTOR_PATH}
    ${MOTOR_PATH}/core
    ${MOTOR_PATH}/graphics
    ${MOTOR_PATH}/resources
)

# Librerias estaticas de third_Party para Android (64 bits)
set(TP_LIB_DIR "${THIRD_PARTY}/Android_64_Bits")
add_library(tp_raylib STATIC IMPORTED)
set_target_properties(tp_raylib PROPERTIES
    IMPORTED_LOCATION ${TP_LIB_DIR}/libraylib.a)

add_library(tp_imgui STATIC IMPORTED)
set_target_properties(tp_imgui PROPERTIES
    IMPORTED_LOCATION ${TP_LIB_DIR}/libimgui_android_64.a)

add_library(tp_jolt STATIC IMPORTED)
set_target_properties(tp_jolt PROPERTIES
    IMPORTED_LOCATION ${TP_LIB_DIR}/libJolt_android_64.a)

add_library(tp_assimp STATIC IMPORTED)
set_target_properties(tp_assimp PROPERTIES
    IMPORTED_LOCATION ${TP_LIB_DIR}/libassimp.a)

# Fuentes
set(SOURCES main.cpp ${COMMON_DIR}/ARX_PlatformImpl.cpp)

# Librerias del sistema Android
find_library(LOG-lib log)
find_library(ANDROID-lib android)
find_library(EGL-lib EGL)
find_library(GLES-lib GLESv2)
find_library(OPENGL-lib GL)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    tp_raylib
    tp_imgui
    tp_jolt
    tp_assimp
    ${OPENGL-lib}
    ${EGL-lib}
    ${GLES-lib}
    ${ANDROID-lib}
    ${LOG-lib}
    m
    pthread
    z
)
