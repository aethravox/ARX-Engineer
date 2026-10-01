# client/CMakeLists.txt
set(ARX_CLIENT_SOURCES
    src/core/main.c
    src/loader/aex_loader.c
    src/sandbox/sandbox.c
    src/runtime/runtime.c
    src/render/renderer.c
    src/audio/audio.c
    src/net/net.c
)

if(WIN32)
    list(APPEND ARX_CLIENT_SOURCES src/platform/windows/platform.c)
elseif(ANDROID)
    list(APPEND ARX_CLIENT_SOURCES src/platform/android/platform.c)
elseif(EMSCRIPTEN)
    list(APPEND ARX_CLIENT_SOURCES src/platform/web/platform.c)
else()
    list(APPEND ARX_CLIENT_SOURCES src/platform/linux/platform.c)
endif()

add_library(arx_client STATIC ${ARX_CLIENT_SOURCES})

target_include_directories(arx_client
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/common/include
)

target_link_libraries(arx_client
    PUBLIC
        arx_common
)

if(ARX_USE_LUAU)
    message(STATUS "ARX Client: Luau pendiente de integrar (submodule)")
endif()

find_package(OpenGL REQUIRED)
target_link_libraries(arx_client PUBLIC OpenGL::GL)

add_executable(arx-client src/main.c)
target_link_libraries(arx-client PRIVATE arx_client)
