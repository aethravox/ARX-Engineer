#!/bin/bash
# zen-docker.sh — Wrapper para correr Zen dentro de Docker
# Uso:
#   ./zen-docker.sh build          # Construir la imagen Docker
#   ./zen-docker.sh compile        # Compilar main.zen dentro de Docker
#   ./zen-docker.sh run <file.zen> # Compilar y ejecutar un .zen
#   ./zen-docker.sh shell          # Abrir shell dentro del container

set -e

DOCKER_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_DIR="/home/aethravox/Escritorio/Projectos/Motores/ARX"

# Verificar que el repo existe
if [ ! -d "$REPO_DIR" ]; then
    echo "ERROR: No se encuentra el repo ARX en $REPO_DIR"
    echo "Edita este script y cambia REPO_DIR"
    exit 1
fi

case "$1" in
    build)
        echo "Construyendo imagen Docker zen-selfhost..."
        docker build -t zen-selfhost:latest "$DOCKER_DIR"
        echo "OK: imagen construida"
        ;;

    compile)
        echo "Compilando main.zen dentro de Docker..."
        docker run --rm \
            --cpus="1.0" \
            --memory="512m" \
            --memory-swap="512m" \
            -v "$REPO_DIR:/zen" \
            zen-selfhost:latest
        ;;

    run)
        if [ -z "$2" ]; then
            echo "Uso: $0 run <archivo.zen> [opciones]"
            exit 1
        fi
        echo "Ejecutando ./main ${@:2} dentro de Docker..."
        docker run --rm \
            --cpus="1.0" \
            --memory="512m" \
            --memory-swap="512m" \
            -v "$REPO_DIR:/zen" \
            zen-selfhost:latest "${@:2}"
        ;;

    shell)
        echo "Abriendo shell en container zen-dev..."
        docker run --rm -it \
            --cpus="1.0" \
            --memory="512m" \
            -v "$REPO_DIR:/zen" \
            --entrypoint /bin/bash \
            zen-selfhost:latest
        ;;

    *)
        echo "Uso: $0 {build|compile|run|shell}"
        echo ""
        echo "Comandos:"
        echo "  build          Construir la imagen Docker (solo primera vez)"
        echo "  compile        Compilar main.zen dentro de Docker"
        echo "  run <file>     Compilar y ejecutar un .zen"
        echo "  shell          Abrir shell interactivo en el container"
        echo ""
        echo "Ejemplos:"
        echo "  $0 build"
        echo "  $0 compile"
        echo "  $0 run ejemplos/hola.zen --ir"
        echo "  $0 shell"
        exit 1
        ;;
esac
