# Zen Self-Hosting Roadmap

Progreso del bootstrap de Zen sobre Zen — el compilador Zen escrito en Zen.

## Estado actual (commit FASE-1-DIAGNOSTICO)

```
✅ Modularización completa (6 archivos .zen con imports)
✅ Compila a binario nativo con LLVM embebido
✅ ./main -h muestra ayuda
✅ ./main archivo.zen lee y procesa sin crashear
✅ Pipeline completo: leer → tokenizar → parsear → codegen → .ll
✅ Exit code 0 limpio
✅ NODO_PRINT implementado en codegen_statement
✅ es_letra/es_digito/es_alfa_num arreglados (retornan 1/0)
✅ Lexer Zen funciona cuando se prueba inline (sin imports)
❌ Lexer Zen NO funciona correctamente cuando se usa con imports
❌ ./main genera hola.ll vacio (solo ret i32 0)
```

---

## FASE 1.6: Fix bug de imports en codegen C++ ⏳ NUEVO — BLOCKER

**Objetivo**: que `tokenizar()` funcione igual con import que sin import

### Bug identificado:

Cuando una función se importa con `importar "lexer"`, el codegen C++
la trata diferente que cuando está inline. Esto causa que `tokenizar()`
genera 2 tokens con import vs 1 token inline.

### Hipótesis:

Las funciones importadas se declaran con firma genérica (todos los
args como `i8*`), mientras que las inline pueden usar tipos nativos
(`double`). Esto cambia cómo se evalúan las comparaciones.

### Pasos:

1. Comparar el IR de `tokenizar()` con import vs sin import
2. Ver cómo el codegen C++ maneja `importar "lexer"`
3. Posible fix: hacer que las funciones importadas mantengan sus tipos
4. O: inline todo el lexer.zen en main.zen (workaround)

### Validación:
```bash
./main ejemplos/hola.zen --ir
# Debe decir "220 tokens generados" (no "6")
```

---

## FASE 1: Codegen de statements básicos ✅ PARCIALMENTE COMPLETO

**Objetivo**: que `./main hola.zen --run` imprima "Hola desde Zen!"

### Progreso:

| # | Nodo | Tipo | Estado |
|---|------|------|--------|
| 1.1 | `NODO_PRINT` | 12 | ✅ Implementado |
| 1.2 | `NODO_ASSIGN` | 10 | ✅ Ya estaba |
| 1.3 | `NODO_EXPR_STMT` | 22 | ✅ Ya estaba |
| 1.4 | `NODO_NUMBER` | 1 | ✅ Ya estaba |
| 1.5 | `NODO_STRING` | 2 | ✅ Ya estaba |
| 1.6 | `NODO_IDENTIFIER` | 5 | ⚠ Bug |
| 1.7 | `NODO_BINARY_OP` | 6 | ✅ Ya estaba |

**Bloqueado por Fase 1.6** (bug de imports)

---

## FASE 2-7: sin cambios

[...]

---

## Resumen actualizado

| Fase | Tiempo | Estado | Notas |
|------|--------|--------|-------|
| 1.6 - Fix bug imports | 2-4 h | ⏳ BLOCKER | Nuevo — se descubrió en Fase 1 |
| 1 - Codegen básico | 1 h más | 🔄 En progreso | Bloqueado por 1.6 |
| 2 - Control flow | 2 h | ⏳ pendiente | |
| 3 - Features avanzadas | 2 h | ⏳ pendiente | |
| 4 - **Bootstrap T-stage** 🔥 | 2-3 h | ⏳ pendiente | |
| 5 - Linker integrado | 1-2 h | ⏳ pendiente | |
| 6 - Android standalone | 1-2 h | ⏳ pendiente | |
| 7 - Release | 1 h | ⏳ pendiente | |
