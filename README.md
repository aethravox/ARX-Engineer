# ARX Engine

> Motor de juegos y apps nativo en C++ con Zen Lang — bilingüe (ES/EN), LLVM AOT + VM, cross-compile standalone.

![License](https://img.shields.io/badge/license-MIT-blue)
![C++](https://img.shields.io/badge/C%2B%2B-20-orange)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20Android-green)

## ¿Qué es?

ARX Engine es un motor de juegos/apps nativo en C++20 con su propio lenguaje de scripting (**Zen**), editor visual integrado, y compilador LLVM embebido que genera ejecutables standalone para Linux, Windows y Android — **sin instalar toolchains externos**.

### Características principales

- **Zen Lang v1.2** — lenguaje bilingüe (español/inglés), sin tipos, sin punto y coma, con LLVM AOT + VM tree-walking
- **Editor visual** — viewport 3D con gizmos (move/rotate/scale), inspector con atributos por tipo, code editor integrado, theme editor, sistema de iconos SVG con hot reload
- **100% standalone** — 1 solo binario (120MB) compila para 3 plataformas sin needing clang/MinGW/NDK instalados
- **Física dual** — Bullet (compat SSE2) + Jolt (modern SSE4.1+) con autodetección
- **Formato .aex** — contenedor de assets firmado Ed25519 con lazy loading
- **Hot reload** — editá `main.zen` y guardá, la VM re-ejecuta automáticamente
- **OpenGL 2.1+** — compatible con hardware viejo (Intel 4500)

## Arquitectura

```
┌──────────────────────────────────────────────────────┐
│  ARX Engine (1 binario, 120MB)                       │
│  ├─ Editor visual (ImGui + docking)                  │
│  │   ├─ Viewport 3D/2D con gizmos                    │
│  │   ├─ Inspector con atributos por tipo de Vox      │
│  │   ├─ Code editor integrado (editar .zen)          │
│  │   ├─ Theme editor (5 presets: ARX/Godot/VS/Light) │
│  │   ├─ FileSystem con drag&drop de assets           │
│  │   └─ Sistema de iconos SVG (nanosvg)              │
│  ├─ Zen Lang (compilador + VM)                       │
│  │   ├─ LLVM estático embebido (cross-compile)       │
│  │   ├─ lld embebido (linker integrado)              │
│  │   └─ Runtimes embebidos (crt+libs 3 plataformas)  │
│  ├─ Motor                                           │
│  │   ├─ Renderer OpenGL 2.1+                        │
│  │   ├─ Física dual (Bullet + Jolt)                  │
│  │   ├─ Scene tree con ~50 tipos de Voxes            │
│  │   └─ Hot reload via VM                            │
│  └─ Export                                          │
│      ├─ .aex (contenedor de assets firmado)          │
│      └─ CLI tools (arx_keygen, arx_pack, arx_verify) │
└──────────────────────────────────────────────────────┘
```

## Terminología

| ARX | Godot | Descripción |
|-----|-------|-------------|
| Vox | Node | Unidad básica de la escena |
| Atributo | Property | Campo editable de un Vox |
| Escena | Scene | Jerarquía de Voxes |
| Zen | GDScript | Lenguaje de scripting |

## Compilar

### Requisitos

- CMake 3.16+
- C++20 (GCC 10+, Clang 12+)
- LLVM 14 (`llvm-14-dev`)
- lld-14 (`lld-14`, `liblld-14-dev`)
- MinGW (`mingw-w64`) — para cross-compile a Windows
- Android NDK — para cross-compile a Android

### Build

```bash
cd engine
mkdir build && cd build
cmake .. -DARX_BUILD_EDITOR=ON
cmake --build . -j$(nproc)
```

### Generar runtimes embebidos (opcional, para 100% standalone)

```bash
cd engine/zen
python3 ../../tools/pack_runtimes.py
# Esto genera zen_runtimes.cpp con crt+libs embebidos
```

## Uso

### Editor

```bash
./build/bin/arx-editor [path/al/proyecto]
```

### Compilar Zen

```bash
# Compilar a ejecutable standalone
./build/bin/zen main.zen --link --platform linux
./build/bin/zen main.zen --link --platform windows
./build/bin/zen main.zen --link --platform android

# Solo generar .o (sin link)
./build/bin/zen main.zen --obj --platform windows
```

### Empaquetar assets en .aex

```bash
# Generar claves
./build/bin/arx_keygen --output myproject

# Empaquetar
./build/bin/arx_pack --input ./assets --output game.aex --priv myproject.priv --name "My Game"

# Verificar
./build/bin/arx_verify --input game.aex --pub myproject.pub
```

## Zen Lang — Ejemplo

```
# Hola mundo en Zen (bilingüe)
funcion saludar(nombre)
    muestra f"Hola {nombre}!"

saludar("mundo")

# Control de flujo
para i desde 1 hasta 10
    si i % 2 == 0
        muestra f"{i} es par"
    sino
        muestra f"{i} es impar"
```

## Estructura del repo

```
ARX/
├── engine/          # Motor + editor + Zen compiler
│   ├── src/         # Código del motor (core, scene, render, physics, editor)
│   ├── zen/         # Zen lang (lexer, parser, codegen, VM, linker, runtimes)
│   ├── thirdparty/  # Dependencias (bullet, nanosvg, freetype, etc.)
│   └── tools/       # CLI tools (arx_keygen, arx_pack, arx_verify, etc.)
├── common/          # Lib compartida (Ed25519, .aex format, manifest)
├── install/         # Scripts de instalación (Linux .desktop, MIME types)
└── docs/            # Documentación
```

## Licencia

MIT — 100% código original.

## Autor

**Aethravox Studios**
