# ==============================================================================
# src/editor/CMakeLists.txt
# ==============================================================================
file(GLOB_RECURSE EDITOR_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(arx_editor_lib STATIC ${EDITOR_SOURCES})

target_include_directories(arx_editor_lib PUBLIC
    if(ARX_PLATFORM_ANDROID)
        target_include_directories(arx_editor_lib PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/fmt-include)
    endif()
    ${ARX_THIRDPARTY}/nanosvg
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${CMAKE_SOURCE_DIR}/../common/include
)

# Link against arx_common (la lib .aex de common/)
# Asumimos que common/ ya fue construido y libarx_common.a está disponible
set(ARX_COMMON_LIB ${CMAKE_SOURCE_DIR}/../common/build/libarx_common.a
    CACHE FILEPATH "Path to libarx_common.a")
if(EXISTS ${ARX_COMMON_LIB})
    target_link_libraries(arx_editor_lib PUBLIC zen_static
        arx_core
        ${ARX_COMMON_LIB}
    )
else()
    message(WARNING "libarx_common.a no encontrado en ${ARX_COMMON_LIB}. "
                    "Ejecutá 'cmake .. && make' en common/ primero.")
    target_link_libraries(arx_editor_lib PUBLIC zen_static arx_core)
endif()

# ImGui core sources + Backend de ImGui para GLFW + OpenGL3.
target_sources(arx_editor_lib PRIVATE
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl2.cpp
)

# Splash + icono embebidos.
target_compile_definitions(arx_editor_lib PUBLIC
    ARX_SPLASH_PATH="${CMAKE_SOURCE_DIR}/src/assets/splash.png"
    ARX_ICON_PATH="${CMAKE_SOURCE_DIR}/src/assets/app_icon.png"
)
