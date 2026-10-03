# Zen Self-Hosting Roadmap

Progreso del bootstrap de Zen sobre Zen — el compilador Zen escrito en Zen.

## Estado actual (commit FASE-1.6-DIAGNOSTICO)

```
✅ Modularización completa (6 archivos .zen con imports)
✅ Compila a binario nativo con LLVM embebido
✅ ./main -h muestra ayuda
✅ Pipeline completo: leer → tokenizar → parsear → codegen → .ll
✅ Lexer Zen funciona (3 tokens para "muestra 42") ✅ NUEVO
✅ Parser Zen funciona (1 nodo tipo PRINT para "muestra 42") ✅ NUEVO
✅ Fix operadores o/y problemáticos en lexer.zen y parser.zen
❌ codegen_statement crashea dentro del branch PRINT (strtod NULL)
```

---

## FASE 1.7: Fix codegen_statement PRINT ⏳ NUEVO — BLOCKER

**Objetivo**: que `codegen_statement` genere el printf para NODO_PRINT

### Bug identificado:

Dentro del branch `si tipo == 12` (PRINT):
```zen
hijos = nodo.hijos          # obtiene lista de hijos
arg = codegen_expresion(estado, hijos[0])   # evalúa primer hijo
```

El crash ocurre en `codegen_expresion(estado, hijos[0])`. Posibles causas:
1. `hijos[0]` retorna NULL (lista vacía o índice inválido)
2. `codegen_expresion` no maneja NODO_NUMBER (tipo 1) correctamente
3. El nodo NUMBER tiene el valor como string pero codegen_expresion espera double

### Pasos:

1. Agregar debug printf dentro del branch PRINT de codegen_statement
2. Verificar que `hijos[0]` retorna un nodo válido (no NULL)
3. Verificar el tipo del nodo retornado (debería ser 1 = NUMBER)
4. Debugear `codegen_expresion` para NODO_NUMBER

### Validación:
```bash
./main /tmp/simple.zen --ir
# simple.zen = "muestra 42"
# Debe generar simple.ll con:
#   define i32 @main() {
#     entry:
#       %0 = call i32 (i8*, ...) @printf(..., double 4.200000e+01)
#       ret i32 0
#   }
```

### Commit esperado: `Zen sobre Zen: Fase 1.7 - Fix codegen_statement PRINT`

---

## FASE 1: Codegen de statements básicos 🔄 80% COMPLETO

**Objetivo**: que `./main hola.zen --run` imprima "Hola desde Zen!"

### Progreso:

| # | Nodo | Tipo | Estado |
|---|------|------|--------|
| 1.1 | `NODO_PRINT` | 12 | 🔄 En progreso (crashea) |
| 1.2 | `NODO_ASSIGN` | 10 | ✅ Implementado |
| 1.3 | `NODO_EXPR_STMT` | 22 | ✅ Implementado |
| 1.4 | `NODO_NUMBER` | 1 | ⚠ Revisar |
| 1.5 | `NODO_STRING` | 2 | ✅ Implementado |
| 1.6 | `NODO_IDENTIFIER` | 5 | ⚠ Revisar |
| 1.7 | `NODO_BINARY_OP` | 6 | ✅ Implementado |

### Commits de la sesión:
- `4de9709` Fix bug crítico del lexer (operadores o/y)
- `7eafc89` Fix operadores o/y en parser.zen
- Este commit: Diagnóstico completo

---

## FASE 2-7: sin cambios

[...]

---

## Resumen actualizado

| Fase | Tiempo | Estado | Notas |
|------|--------|--------|-------|
| 1.7 - Fix codegen PRINT | 1-2 h | ⏳ BLOCKER | Nuevo — bug en codegen_expresion |
| 1 - Codegen básico | casi listo | 🔄 80% | Bloqueado por 1.7 |
| 2 - Control flow | 2 h | ⏳ pendiente | |
| 3 - Features avanzadas | 2 h | ⏳ pendiente | |
| 4 - **Bootstrap T-stage** 🔥 | 2-3 h | ⏳ pendiente | |
| 5 - Linker integrado | 1-2 h | ⏳ pendiente | |
| 6 - Android standalone | 1-2 h | ⏳ pendiente | |
| 7 - Release | 1 h | ⏳ pendiente | |

## Logros de la sesión

1. ✅ Identificado el bug raíz: operadores `o`/`y` no funcionan en Zen
2. ✅ Arreglado el lexer.zen (reemplazados todos los o/y con si anidados)
3. ✅ Arreglado el parser.zen (reemplazados todos los o/y con flags)
4. ✅ Lexer Zen funciona: genera tokens correctos
5. ✅ Parser Zen funciona: genera AST correcto
6. ✅ Diagnóstico del bug restante: codegen_statement PRINT
