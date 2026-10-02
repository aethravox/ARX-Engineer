# src/ — Código fuente de Zen escrito EN Zen

Este directorio contiene los componentes de Zen reescritos en Zen mismo.
A medida que cada componente se completa, el equivalente en C++ se borra de src_Old/.

## Estado actual

| Componente | Archivo Zen | Equivalente C++ (src_Old) | Estado |
|-----------|-------------|--------------------------|--------|
| Builtins | builtins.zen | codegen.cpp (builtins) | ✅ Parcial (muestra, longitud) |
| Lexer | lexer.zen | lexer.cpp, lexer.h | 🔄 En progreso (80%) |
| Parser | (pendiente) | parser.cpp, parser.h | ❌ Pendiente |
| AST | (pendiente) | ast.h | ❌ Pendiente |
| Codegen | (pendiente) | codegen.cpp | ❌ Pendiente |
| Main | (pendiente) | main.cpp | ❌ Pendiente |

## Regla
Cuando un componente en Zen esté 100% completo y probado:
1. Borrar el archivo C++ correspondiente de src_Old/
2. Actualizar este README
3. Commit + push

Cuando src_Old/ esté vacío = Zen sobre Zen completo.
