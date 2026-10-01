# ==============================================================================
# src/CMakeLists.txt — núcleo del motor y submódulos
# ==============================================================================

# ---- Biblioteca estática principal: arx_core -------------------------------
# Contiene todo el código C++ del motor (sin main). Tanto el editor como el
# runtime la enlazan. Esto permite que el mismo binario exportado use exactamente
# las mismas bibliotecas que el editor (requisito del usuario).
file(GLOB_RECURSE ARX_CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/os/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/render/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scene/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/physics/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/audio/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/input/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/net/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/arxscript/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/export/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/*.cpp
)
# NOTA: physics_nodes_2d.cpp y physics_nodes_3d.cpp se incluyen de nuevo.
# Se editaron para eliminar SOLO la implementación duplicada de
# RigidBody2D/3D::_physics_process (que ya está en physics/nodes_*_physics.cpp).
# Las demás implementaciones (_enter_tree, _exit_tree, move_and_slide) son únicas
# de estos archivos y necesarias para evitar errores de vtable.

# Plataforma específica (se compila solo la plataforma activa)
if(ARX_PLATFORM_WINDOWS)
    file(GLOB ARX_PLATFORM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/platforms/windows/*.cpp)
elseif(ARX_PLATFORM_LINUX)
    file(GLOB ARX_PLATFORM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/platforms/linux/*.cpp)
elseif(ARX_PLATFORM_ANDROID)
    file(GLOB ARX_PLATFORM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/platforms/android/*.cpp)
elseif(ARX_PLATFORM_WEB)
    file(GLOB ARX_PLATFORM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/platforms/web/*.cpp)
endif()

add_library(arx_core STATIC ${ARX_CORE_SOURCES} ${ARX_PLATFORM_SOURCES})

target_include_directories(arx_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ARX_THIRDPARTY}
    ${ARX_THIRDPARTY}/glad
    ${ARX_THIRDPARTY}/glad/glad
    ${ARX_THIRDPARTY}/bullet
    ${ARX_THIRDPARTY}/freetype/include
    ${ARX_THIRDPARTY}/enet
    ${ARX_THIRDPARTY}/enet/enet
    ${ARX_THIRDPARTY}/mbedtls/include
    ${ARX_THIRDPARTY}/minimp3
    ${ARX_THIRDPARTY}/libogg/include
    ${ARX_THIRDPARTY}/libvorbis/include
    ${ARX_THIRDPARTY}/nanosvg
    ${ARX_THIRDPARTY}/recastnavigation
    ${ARX_THIRDPARTY}/recastnavigation/Recast
    ${ARX_THIRDPARTY}/recastnavigation/Detour
    ${ARX_THIRDPARTY}/zlib
    ${ARX_THIRDPARTY}/zstd
    ${ARX_THIRDPARTY}/zstd/lib
    ${ARX_THIRDPARTY}/zstd/lib/common
)

# GLM necesita GLM_ENABLE_EXPERIMENTAL para gtx/quaternion, gtx/norm, gtx/component_wise.
target_compile_definitions(arx_core PUBLIC GLM_ENABLE_EXPERIMENTAL)

# _GNU_SOURCE: necesario en GCC 11 con glibc para que <ctime> exponga clock(), CLOCK_MONOTONIC, etc.
# Sin esto, <mutex> falla porque CLOCK_MONOTONIC no está declarado.
if(UNIX AND NOT APPLE)
    target_compile_definitions(arx_core PUBLIC _GNU_SOURCE)
endif()

# miniaudio include path
target_include_directories(arx_core PUBLIC ${miniaudio_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/../common/include)
target_compile_definitions(arx_core PUBLIC
    ARX_VERSION="${ARX_VERSION_STRING}"
    ARX_PLATFORM_${ARX_PLATFORM}=1
    ARX_PLATFORM="${ARX_PLATFORM}"
    $<$<CONFIG:Debug>:ARX_DEBUG=1>
    $<$<BOOL:ARX_VERBOSE_LOG>:ARX_VERBOSE_LOG=1>
)

##target_compile_features(arx_core PUBLIC cxx_std_20)
target_compile_options(arx_core PRIVATE ${ARX_CORE_COMPILE_FLAGS})

# Dependencias (siempre glm + stb + fmt; glfw solo en desktop)
target_link_libraries(arx_core PUBLIC glm::glm fmt::fmt)

# glad (path real: thirdparty/glad/glad.c)
target_sources(arx_core PRIVATE ${ARX_THIRDPARTY}/glad/glad.c)

# Bullet physics (3D)
if(ARX_USE_BULLET)
    file(GLOB_RECURSE BULLET_SOURCES
        ${ARX_THIRDPARTY}/bullet/BulletCollision/*.cpp
        ${ARX_THIRDPARTY}/bullet/BulletDynamics/*.cpp
        ${ARX_THIRDPARTY}/bullet/LinearMath/*.cpp
    )
    target_sources(arx_core PRIVATE ${BULLET_SOURCES})
    target_compile_definitions(arx_core PUBLIC ARX_BULLET_ENABLED=1)
endif()

# FreeType
if(ARX_USE_FREETYPE)
    file(GLOB FREETYPE_SOURCES
        ${ARX_THIRDPARTY}/freetype/src/base/*.c
        ${ARX_THIRDPARTY}/freetype/src/autofit/*.c
        ${ARX_THIRDPARTY}/freetype/src/bdf/*.c
        ${ARX_THIRDPARTY}/freetype/src/bzip2/*.c
        ${ARX_THIRDPARTY}/freetype/src/cache/*.c
        ${ARX_THIRDPARTY}/freetype/src/cff/*.c
        ${ARX_THIRDPARTY}/freetype/src/cid/*.c
        ${ARX_THIRDPARTY}/freetype/src/gzip/*.c
        ${ARX_THIRDPARTY}/freetype/src/lzw/*.c
        ${ARX_THIRDPARTY}/freetype/src/pcf/*.c
        ${ARX_THIRDPARTY}/freetype/src/pfr/*.c
        ${ARX_THIRDPARTY}/freetype/src/psaux/*.c
        ${ARX_THIRDPARTY}/freetype/src/pshinter/*.c
        ${ARX_THIRDPARTY}/freetype/src/psnames/*.c
        ${ARX_THIRDPARTY}/freetype/src/raster/*.c
        ${ARX_THIRDPARTY}/freetype/src/sfnt/*.c
        ${ARX_THIRDPARTY}/freetype/src/smooth/*.c
        ${ARX_THIRDPARTY}/freetype/src/truetype/*.c
        ${ARX_THIRDPARTY}/freetype/src/type1/*.c
        ${ARX_THIRDPARTY}/freetype/src/winfonts/*.c
    )
    # Excluir archivos que no compilan en Linux
    list(REMOVE_ITEM FREETYPE_SOURCES
        ${ARX_THIRDPARTY}/freetype/src/base/ftmac.c
    )
    # Excluir "aggregator" files de FreeType. Estos archivos (autofit.c, smooth.c,
    # raster.c, truetype.c, etc.) son "single-file builds" que #incluyen a los
    # .c individuales del mismo directorio. Si los compilamos ambos, tenemos
    # múltiples definiciones de los mismos símbolos (af_*, ft_raster1_*, etc.)
    # al enlazar arx-runtime / arx-editor. La regla: para cada subdirectorio
    # de freetype/src/, existe un <nombre>.c que es aggregator - excluirlo
    # y dejar solo los .c individuales.
    list(REMOVE_ITEM FREETYPE_SOURCES
        ${ARX_THIRDPARTY}/freetype/src/autofit/autofit.c
        ${ARX_THIRDPARTY}/freetype/src/bdf/bdf.c
        ${ARX_THIRDPARTY}/freetype/src/cache/ftcache.c
        ${ARX_THIRDPARTY}/freetype/src/cff/cff.c
        ${ARX_THIRDPARTY}/freetype/src/cid/type1cid.c
        ${ARX_THIRDPARTY}/freetype/src/pcf/pcf.c
        ${ARX_THIRDPARTY}/freetype/src/pfr/pfr.c
        ${ARX_THIRDPARTY}/freetype/src/psaux/psaux.c
        ${ARX_THIRDPARTY}/freetype/src/pshinter/pshinter.c
        ${ARX_THIRDPARTY}/freetype/src/psnames/psmodule.c
        ${ARX_THIRDPARTY}/freetype/src/raster/raster.c
        ${ARX_THIRDPARTY}/freetype/src/sfnt/sfnt.c
        ${ARX_THIRDPARTY}/freetype/src/smooth/smooth.c
        ${ARX_THIRDPARTY}/freetype/src/truetype/truetype.c
        ${ARX_THIRDPARTY}/freetype/src/type1/type1.c
        ${ARX_THIRDPARTY}/freetype/src/winfonts/winfonts.c
    )
    target_sources(arx_core PRIVATE ${FREETYPE_SOURCES})
    target_compile_definitions(arx_core PUBLIC
        ARX_FREETYPE_ENABLED=1
        FT2_BUILD_LIBRARY=1
    )
endif()

# ENet (networking)
if(ARX_USE_ENET)
    file(GLOB ENET_SOURCES ${ARX_THIRDPARTY}/enet/*.c)
    # Excluir platform-specific (los manejamos vía defines)
    list(REMOVE_ITEM ENET_SOURCES
        ${ARX_THIRDPARTY}/enet/unix.c
        ${ARX_THIRDPARTY}/enet/win32.c
        ${ARX_THIRDPARTY}/enet/godot.cpp
    )
    target_sources(arx_core PRIVATE ${ENET_SOURCES})
    target_compile_definitions(arx_core PUBLIC ARX_ENET_ENABLED=1 HAS_SOCKLEN_T=1)
    if(WIN32)
        target_link_libraries(arx_core PUBLIC winmm ws2_32)
    endif()
endif()

# zlib (necesario para FreeType y minizip)
file(GLOB ZLIB_SOURCES
    ${ARX_THIRDPARTY}/zlib/adler32.c
    ${ARX_THIRDPARTY}/zlib/compress.c
    ${ARX_THIRDPARTY}/zlib/crc32.c
    ${ARX_THIRDPARTY}/zlib/deflate.c
    ${ARX_THIRDPARTY}/zlib/gzclose.c
    ${ARX_THIRDPARTY}/zlib/gzlib.c
    ${ARX_THIRDPARTY}/zlib/gzread.c
    ${ARX_THIRDPARTY}/zlib/gzwrite.c
    ${ARX_THIRDPARTY}/zlib/inflate.c
    ${ARX_THIRDPARTY}/zlib/inffast.c
    ${ARX_THIRDPARTY}/zlib/inftrees.c
    ${ARX_THIRDPARTY}/zlib/trees.c
    ${ARX_THIRDPARTY}/zlib/uncompr.c
    ${ARX_THIRDPARTY}/zlib/zutil.c
)
target_sources(arx_core PRIVATE ${ZLIB_SOURCES})

# minizip
file(GLOB MINIZIP_SOURCES ${ARX_THIRDPARTY}/minizip/*.c)
target_sources(arx_core PRIVATE ${MINIZIP_SOURCES})

# Desktop: GLFW + OpenGL
if(NOT EMSCRIPTEN AND NOT ANDROID)
    target_link_libraries(arx_core PUBLIC glfw)
    if(WIN32)
        target_link_libraries(arx_core PUBLIC opengl32 gdi32 user32)
    elseif(APPLE)
        target_link_libraries(arx_core PUBLIC "-framework OpenGL")
    else()
        target_link_libraries(arx_core PUBLIC GL X11 Xcursor Xinerama Xrandr Xi dl)
    endif()
endif()

# Web (Emscripten): GL es GLES2, no GLFW
if(EMSCRIPTEN)
    target_link_libraries(arx_core PUBLIC "-sUSE_GLFW=3" "-sFULL_ES3=1")
endif()

# Android: GLESv3 + log + android
if(ANDROID)
    target_link_libraries(arx_core PUBLIC GLESv3 EGL log android)
endif()

# ---- Editor ( Dear ImGui ) --------------------------------------------------
if(ARX_BUILD_EDITOR)
    add_subdirectory(editor)
    add_executable(arx_editor ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
    target_link_libraries(arx_editor PRIVATE arx_core arx_editor_lib)
    set_target_properties(arx_editor PROPERTIES
        OUTPUT_NAME "arx-editor"
        VS_DPI_AWARE "PerMonitor"
        WIN32_EXECUTABLE ON
        MACOSX_BUNDLE ON)
    if(EMSCRIPTEN)
        set_target_properties(arx_editor PROPERTIES LINK_FLAGS
            "-sALLOW_MEMORY_GROWTH=1 -sUSE_GLFW=3 -sFULL_ES3=1")
    endif()
endif()

# ---- Runtime (sin editor, para export) --------------------------------------
if(ARX_BUILD_RUNTIME)
    add_executable(arx_runtime ${CMAKE_CURRENT_SOURCE_DIR}/runtime_main.cpp)
    target_link_libraries(arx_runtime PRIVATE arx_core)
    set_target_properties(arx_runtime PROPERTIES OUTPUT_NAME "arx-runtime")
endif()

# ---- Instalación ------------------------------------------------------------
include(GNUInstallDirs)
install(TARGETS arx_core
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/arx
    FILES_MATCHING PATTERN "*.hpp")
target_include_directories(arx_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/stb)
target_sources(arx_core PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/render/stb_image_impl.cpp)
target_compile_options(arx_core PRIVATE -Wno-unused-function -Wno-unused-variable -Wno-deprecated-declarations)
