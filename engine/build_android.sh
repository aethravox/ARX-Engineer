#!/bin/bash
# ==============================================================================
# build_android.sh — Cross-compile ARX Engine editor para Android ARM64
#
# Genera: libarx_engine.so (NativeActivity) + APK
# ==============================================================================
set -e

NDK="/home/aethravox/Escritorio/p/mis_cosas/Android/linux/android-ndk-r29"
REPO="/home/aethravox/Escritorio/Projectos/Motores/ARX"
BUILD_DIR="$REPO/engine/build-android"
SDK="/home/aethravox/Escritorio/p/mis_cosas/Android/linux"

echo "=== ARX Engine Android Build ==="
echo "NDK: $NDK"
echo "ABI: arm64-v8a"
echo "API: 24"
echo ""

# 1. Crear directorio de build
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# 2. CMake configure
echo "=== CMake Configure ==="
cmake "$REPO/engine" \
    -DCMAKE_TOOLCHAIN_FILE="$NDK/build/cmake/android.toolchain.cmake" \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-24 \
    -DANDROID_NDK="$NDK" \
    -DARX_PLATFORM_ANDROID=ON \
    -DARX_BUILD_EDITOR=ON \
    -DCMAKE_BUILD_TYPE=Release \
    2>&1 | tail -20

# 3. Build
echo ""
echo "=== Building ==="
cmake --build . -j$(nproc) 2>&1 | tail -20

# 4. Verificar .so
echo ""
echo "=== Output ==="
ls -la "$BUILD_DIR/lib/libarx_engine.so" 2>/dev/null || echo "ERROR: libarx_engine.so no encontrado"

# 5. Crear APK (si tenemos las tools)
AAPT=$(which aapt 2>/dev/null || which aapt2 2>/dev/null)
ZIPALIGN=$(which zipalign 2>/dev/null)
APKSIGNER=$(which apksigner 2>/dev/null)

if [ -n "$AAPT" ]; then
    echo ""
    echo "=== Packaging APK ==="
    APK_DIR="$BUILD_DIR/apk"
    mkdir -p "$APK_DIR/lib/arm64-v8a"
    
    # Copiar .so
    cp "$BUILD_DIR/lib/libarx_engine.so" "$APK_DIR/lib/arm64-v8a/"
    
    # Crear APK base
    $AAPT package -f -F "$BUILD_DIR/arx-engine-unsigned.apk" \
        -M "$REPO/engine/install/AndroidManifest.xml" \
        -I "$SDK/platforms/android-34/android.jar" \
        -A "$APK_DIR" \
        2>&1 || echo "aapt falló - necesitas android.jar"
    
    echo "APK: $BUILD_DIR/arx-engine-unsigned.apk"
else
    echo "⚠ aapt no encontrado. Instala: sudo apt install aapt"
    echo "  .so generado en: $BUILD_DIR/lib/libarx_engine.so"
fi

echo ""
echo "=== Done ==="
