#!/bin/bash
# run_zen.sh — Script que corre dentro del container Docker
# Construye el compilador C++ si no existe, luego compila y ejecuta main.zen
# Todo con timeout para evitar colgarse

set -e

ZEN_DIR="/zen/engine/zen"
BUILD_DIR="$ZEN_DIR/build"

echo "=== Zen Docker Container ==="
echo "Working directory: $ZEN_DIR"
echo ""

# 1. Construir el compilador C++ si no existe
if [ ! -f "$BUILD_DIR/zen" ]; then
    echo "[1/3] Construyendo compilador C++..."
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    cmake .. -DCMAKE_BUILD_TYPE=Release 2>&1 | tail -5
    make -j$(nproc) 2>&1 | tail -5
    echo "   OK: build/zen construido"
else
    echo "[1/3] Compilador C++ ya existe, saltando build"
fi

# 2. Compilar main.zen a .o
echo "[2/3] Compilando src/main.zen..."
cd "$ZEN_DIR"
rm -f main main.o main.ll
timeout 30 ./build/zen src/main.zen --obj 2>&1 | tail -5
if [ ! -f main.o ]; then
    echo "   ERROR: no se genero main.o"
    exit 1
fi
echo "   OK: main.o generado"

# 3. Linkear con libzen.a (que tiene los wrappers zen_call*)
echo "[3/3] Linkeando main..."
cc main.o -o main \
    $BUILD_DIR/libzen.a \
    $(llvm-config-14 --libs core executionengine native) \
    $(llvm-config-14 --system-libs) \
    -lm -lstdc++ -lpthread 2>&1 | tail -3
echo "   OK: main linkeado"
echo ""

# Ejecutar con timeout
if [ -n "$1" ]; then
    echo "=== Ejecutando: ./main $@ ==="
    timeout 10 ./main "$@"
    echo "=== Exit code: $? ==="
else
    echo "=== main construido. Uso: docker run ... <archivo.zen> --ir ==="
fi
