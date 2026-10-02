# common/CMakeLists.txt
#
# Construye la libreria arx_common que contiene:
#   - arx::format   (lector + builder del .aex)
#   - arx::crypto   (Ed25519 + SHA-256)
#
# Ed25519: usa libsodium si ARX_USE_SODIUM=ON y se encuentra.
#          Si no, usa la implementacion embebida ed25519_donna (orlp/ed25519).

cmake_minimum_required(VERSION 3.20)
project(arx_common LANGUAGES C CXX VERSION 0.0.1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ---- Dependencias opcionales -------------------------------------------------
find_package(Threads REQUIRED)
find_package(ZLIB)
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(SODIUM IMPORTED_TARGET libsodium)
endif()
pkg_check_modules(zstd IMPORTED_TARGET libzstd)

# ---- Fuentes -----------------------------------------------------------------
set(ARX_COMMON_SOURCES
    src/format/aex.c
    src/format/aex_builder.c
    src/format/manifest.c
    src/format/sandbox.c
    src/format/tar.c
    src/crypto/sha256.c
    src/crypto/ed25519.c
)

# Si no usamos libsodium, incluir la implementacion embebida ed25519_donna
if(NOT (ARX_USE_SODIUM AND SODIUM_FOUND))
    message(STATUS "ARX: usando ed25519_donna embebida (sin libsodium)")
    list(APPEND ARX_COMMON_SOURCES
        src/crypto/ed25519_donna/sha512_donna.c
        src/crypto/ed25519_donna/fe.c
        src/crypto/ed25519_donna/ge.c
        src/crypto/ed25519_donna/sc.c
        src/crypto/ed25519_donna/keypair.c
        src/crypto/ed25519_donna/sign.c
        src/crypto/ed25519_donna/verify.c
        src/crypto/ed25519_donna/seed.c
    )
else()
    message(STATUS "ARX: usando libsodium para Ed25519")
endif()

# ---- Library target ----------------------------------------------------------
add_library(arx_common STATIC ${ARX_COMMON_SOURCES})

# Ahora sí, después de add_library, podemos añadir compile definitions
if(NOT (ARX_USE_SODIUM AND SODIUM_FOUND))
    target_compile_definitions(arx_common PUBLIC ARX_CRYPTO_USE_SODIUM=0)
else()
    target_compile_definitions(arx_common PUBLIC ARX_CRYPTO_USE_SODIUM=1)
endif()

target_include_directories(arx_common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/crypto/ed25519_donna
)

if(ARX_USE_SODIUM AND SODIUM_FOUND)
    target_link_libraries(arx_common PUBLIC PkgConfig::SODIUM)
endif()

if(zstd_FOUND)
    target_link_libraries(arx_common PUBLIC PkgConfig::zstd)
else()
    message(WARNING "zstd no encontrado — assets no se comprimiran")
endif()

if(ZLIB_FOUND)
    target_link_libraries(arx_common PUBLIC ZLIB::ZLIB)
endif()

target_link_libraries(arx_common PUBLIC Threads::Threads)
