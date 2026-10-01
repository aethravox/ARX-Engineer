# ==============================================================================
# engine/cmake/android_build.cmake — Config para cross-compile Android ARM64
#
# Uso:
#   cmake -DCMAKE_TOOLCHAIN_FILE=cmake/android_build.cmake \
#         -DANDROID_NDK=/path/to/ndk \
#         -DANDROID_ABI=arm64-v8a \
#         -DANDROID_PLATFORM=android-24 \
#         ..
# ==============================================================================

# Usar el toolchain del NDK
set(ANDROID_NDK_HOME $ENV{ANDROID_NDK_HOME})
if(NOT ANDROID_NDK_HOME)
    set(ANDROID_NDK_HOME "/home/aethravox/Escritorio/p/mis_cosas/Android/linux/android-ndk-r29")
endif()

message(STATUS "ARX Android: NDK = ${ANDROID_NDK_HOME}")

# Incluir el toolchain del NDK
include(${ANDROID_NDK_HOME}/build/cmake/android.toolchain.cmake)

# android_native_app_glue
set(NATIVE_APP_GLUE_DIR ${ANDROID_NDK_HOME}/sources/android/native_app_glue)

# Android platform
set(ANDROID_PLATFORM android-24)
set(ANDROID_ABI arm64-v8a)

# Libraries
set(ANDROID_LIBS
    android
    log
    EGL
    GLESv2
)

# Include paths for Android
set(ANDROID_INCLUDES
    ${ANDROID_NDK_HOME}/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include
    ${NATIVE_APP_GLUE_DIR}
)
