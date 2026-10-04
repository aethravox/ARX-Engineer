# src/ — Código fuente de Zen escrito EN Zen

Este directorio contiene los componentes de Zen reescritos en Zen mismo.
Cada modulo es importable via `importar "nombre"` y se resuelve en el
mismo directorio (`src/nombre.zen`).

## Estructura modular

```
src/
  ast.zen        Structs (Nodo, Token, CodeGenState), constantes
                 (NODO_*, TIPO_*), constructores y accessors.
                 Sin dependencias.

  lexer.zen      Funcion tokenizar(codigo) -> lista[Token].
                 Importa: ast.

  parser.zen     Funcion parsear(tokens) -> lista[Nodo].
                 Importa: ast, lexer.

  codegen.zen    Funciones codegen_* (LLVM IR generation).
                 Importa: ast.

  builtins.zen   FFI externs (libc, libm).
                 Los builtins nativos (muestra, longitud, numero,
                 leer_archivo, argv, etc.) estan en codegen.cpp y NO
                 se pueden redefinir aqui.

  main.zen       Entry point del CLI. Importa todos los modulos y
                 orquesta el flujo: leer -> tokenizar -> parsear ->
                 codegen -> compilar.

  README.md      Este archivo.
```

## Estado del self-hosting

| Componente | Archivo Zen | Equivalente C++ | Estado |
|-----------|-------------|-----------------|--------|
| AST       | ast.zen      | ast.h           | ✅ Completo |
| Lexer     | lexer.zen    | lexer.cpp/.h    | ✅ Completo (fix bug recursión) |
| Parser    | parser.zen   | parser.cpp/.h   | ✅ Completo |
| Codegen   | codegen.zen  | codegen.cpp     | 🔄 ~90% (algunos builtins faltan) |
| Builtins  | builtins.zen | (en codegen)    | ⚠ FFI only (builtins nativos en C++) |
| Main/CLI  | main.zen     | main.cpp        | ✅ Completo (modularizado) |

## Compilar

```bash
# Con el compilador C++ (./build/zen):
./build/zen src/main.zen --obj
cc main.o -o main $(llvm-config-14 --libs core executionengine native) \
              $(llvm-config-14 --system-libs) -lm -lstdc++ -lpthread

# Probar:
./main ejemplos/hola.zen --ir
./main ejemplos/hola.zen --build
./main ejemplos/hola.zen --run
```

## Bootstrap T-stage (próximo objetivo)

1. `./build/zen src/main.zen --build` → genera `./main` (Zen-on-Zen stage-2) ✅
2. `./main src/main.zen --build` → genera `./main2` (Zen-on-Zen-on-Zen stage-3) ⚠ pendiente
3. Si `./main2` funciona igual que `./main` → bootstrap cerrado 🔥

## Regla

Cuando un componente en Zen esté 100% completo y probado:
1. Borrar el archivo C++ correspondiente de `../src_Old/`
2. Actualizar este README
3. Commit + push

Cuando `src_Old/` esté vacío = Zen sobre Zen completo.
