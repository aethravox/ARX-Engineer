# ==============================================================================
# src/editor/CMakeLists.txt
# ==============================================================================
file(GLOB_RECURSE EDITOR_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(arx_editor_lib STATIC ${EDITOR_SOURCES})

target_include_directories(arx_editor_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(arx_editor_lib PUBLIC
    arx_core
    imgui
)

# Backend de ImGui para GLFW + OpenGL3.
target_sources(arx_editor_lib PRIVATE
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# Splash + icono embebidos.
target_compile_definitions(arx_editor_lib PUBLIC
    ARX_SPLASH_PATH="${CMAKE_SOURCE_DIR}/src/assets/splash.png"
    ARX_ICON_PATH="${CMAKE_SOURCE_DIR}/src/assets/app_icon.png"
)
