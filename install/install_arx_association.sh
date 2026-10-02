#!/bin/bash
# install_arx_association.sh — Instala la asociación de archivos .arx con ARX Engine
# Ejecutar una sola vez: bash install_arx_association.sh

set -e

ARX_DIR="/home/aethravox/Escritorio/Aethravox_Studios/Projectos/Motores/ARX"
BIN="$ARX_DIR/engine/build/bin/arx-editor"
ICON="$ARX_DIR/engine/src/assets/app_icon.png"

echo "=== Instalando asociación de archivos ARX ==="

# 1. Crear directorio de MIME types si no existe
mkdir -p ~/.local/share/mime/packages

# 2. Copiar el MIME type
cp "$ARX_DIR/install/arx-project.xml" ~/.local/share/mime/packages/
echo "✓ MIME type instalado (application/x-arx-project)"

# 3. Actualizar la base de datos de MIME types
update-mime-database ~/.local/share/mime 2>/dev/null || true
echo "✓ Base de datos MIME actualizada"

# 4. Crear directorio de aplicaciones si no existe
mkdir -p ~/.local/share/applications

# 5. Copiar el .desktop file
cp "$ARX_DIR/install/arx-engine.desktop" ~/.local/share/applications/
echo "✓ .desktop file instalado"

# 6. Actualizar el icono
mkdir -p ~/.local/share/icons/hicolor/256x256/apps
cp "$ICON" ~/.local/share/icons/hicolor/256x256/apps/arx-engine.png 2>/dev/null || true

# 7. Actualizar base de datos de aplicaciones
update-desktop-database ~/.local/share/applications 2>/dev/null || true
echo "✓ Base de datos de aplicaciones actualizada"

# 8. Asociar .arx con ARX Engine (default application)
xdg-mime default arx-engine.desktop application/x-arx-project 2>/dev/null || true
echo "✓ .arx asociado con ARX Engine"

echo ""
echo "=== Listo! ==="
echo "Ahora podés:"
echo "  - Doble click en cualquier archivo .arx → abre ARX Editor"
echo "  - Click derecho → 'Abrir con' → ARX Engine"
echo "  - El icono de ARX aparece en el menú de aplicaciones"
echo ""
echo "Para desinstalar:"
echo "  rm ~/.local/share/mime/packages/arx-project.xml"
echo "  rm ~/.local/share/applications/arx-engine.desktop"
echo "  update-mime-database ~/.local/share/mime"
echo "  update-desktop-database ~/.local/share/applications"
