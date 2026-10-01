# Zen Compiler v1.2 — Compilador Nativo con LLVM

Compilador del lenguaje Zen a codigo de maquina nativo usando LLVM.
**Sin dependencia de Clang.** Compilacion 100% nativa.

## Novedades v1.2

- **`#lang es` / `#lang en`** — Forzar idioma por archivo
- **`plataforma` / `platform`** — Compilacion condicional por SO
- **`mientras` / `while`** — Bucles while
- **`para cada x en rango()` / `for each x in range()`** — For-each
- **`repetir N veces` / `repeat N times`** — Repeat
- **`romper` / `break`** — Salir de un bucle
- **`continuar` / `continue`** — Saltar a la siguiente iteracion
- **Ternario** — `cond ? a : b`
- **Asignacion aumentada** — `x += 5`, `x -= 3`, etc.

## Requisitos

- **LLVM 14+** (14, 15, 16, 17)
- **CMake 3.16+**
- **C++17**
- **cc** (linker del sistema — gcc, clang, etc.)

### Instalar en Ubuntu/Debian:

```bash
sudo apt install llvm libllvm-dev cmake build-essential
```

### Instalar en Arch Linux:

```bash
sudo pacman -S llvm cmake
```

### Instalar en macOS:

```bash
brew install llvm cmake
```

## Compilar el compilador

```bash
cd zen-compiler
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Usar

```bash
# Generar LLVM IR (texto)
./zen archivo.zen

# Ver el IR generado
./zen archivo.zen --ir

# Generar codigo objeto (.o)
./zen archivo.zen --obj

# Compilar a ejecutable nativo
./zen archivo.zen --build

# Compilar y ejecutar
./zen archivo.zen --run

# Compilar para una plataforma especifica
./zen archivo.zen --platform linux --run
./zen archivo.zen --platform windows --build
```

## Sintaxis

### #lang — Idioma por archivo

```zen
#lang es    # Solo espanol en este archivo
muestra "Hola"
```

```zen
#lang en    # Solo ingles en este archivo
show "Hello"
```

Sin `#lang`, el archivo acepta ambos idiomas mezclados.

### Plataforma — Compilacion condicional

```zen
plataforma windows
    muestra "Ejecutando en Windows!"

plataforma linux
    muestra "Ejecutando en Linux!"

plataforma macos
    muestra "Ejecutando en macOS!"

plataforma android
    muestra "Ejecutando en Android!"

muestra "Esto siempre se ejecuta"
```

Compilar con `--platform <nombre>` para seleccionar que bloque se incluye.

### Mientras / While

```zen
x = 0
mientras x < 10
    muestra x
    x += 1
```

### Para cada / For each

```zen
para cada i en rango(1, 10)
    muestra i
```

### Repetir / Repeat

```zen
repetir 5 veces
    muestra "Zen!"
```

### Break / Continue

```zen
para i desde 1 hasta 100
    si i == 10
        romper
    muestra i
```

### Ternario

```zen
x = 15
msg = x > 10 ? "grande" : "pequeno"
muestra msg
```

### Asignacion aumentada

```zen
x = 10
x += 5    # x = 15
x -= 3    # x = 12
x *= 2    # x = 24
x /= 4    # x = 6
x %= 4    # x = 2
x ^= 3    # x = 8
```

## Estructura del proyecto

```
zen-compiler/
  src/
    ast.h          Nodos del AST (22 tipos)
    lexer.h/cpp     Tokenizador con #lang
    parser.h/cpp    Parser (while, for-each, repeat, break, continue, ternario, plataforma)
    codegen.h/cpp   Generador LLVM IR + objeto nativo
    main.cpp        CLI con --platform
  runtime.c       (Legacy — runtime embebido en codegen ahora)
  ejemplos/
    hola.zen         Ejemplo en espanol
    english.zen      Ejemplo en ingles
    plataforma.zen   Demo de plataformas y features nuevas
    platform_en.zen  Platform demo in English
  CMakeLists.txt
  README.md
```