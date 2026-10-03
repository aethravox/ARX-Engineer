# Zen Self-Hosting Roadmap

Progreso del bootstrap de Zen sobre Zen — el compilador Zen escrito en Zen.

## Estado actual (commit FASE-1-PARTIAL)

```
✅ Modularización completa (6 archivos .zen con imports)
✅ Compila a binario nativo con LLVM embebido
✅ ./main -h muestra ayuda
✅ ./main archivo.zen lee y procesa sin crashear
✅ Pipeline completo: leer → tokenizar → parsear → codegen → .ll
✅ Exit code 0 limpio
✅ NODO_PRINT implementado en codegen_statement (con printf)
❌ Lexer Zen (tokenizar en lexer.zen) solo genera 6 tokens (C++ genera 220)
❌ Parser Zen recibe solo 6 tokens → AST con 2 nodos (deberia tener 30+)
❌ hola.ll generado tiene solo "ret i32 0" (sin los printf de muestra)
```

---

## FASE 1: Codegen de statements básicos ⏳ EN PROGRESO

**Objetivo**: que `./main hola.zen --run` imprima "Hola desde Zen!"

### Progreso:

| # | Nodo | Tipo | Estado | Notas |
|---|------|------|--------|-------|
| 1.1 | `NODO_PRINT` | 12 | ✅ Implementado | Genera printf("%s\n", arg) |
| 1.2 | `NODO_ASSIGN` | 10 | ✅ Ya estaba | alloca + store |
| 1.3 | `NODO_EXPR_STMT` | 22 | ✅ Ya estaba | evalúa y descarta |
| 1.4 | `NODO_NUMBER` | 1 | ✅ Ya estaba | LLVMConstReal |
| 1.5 | `NODO_STRING` | 2 | ✅ Ya estaba | LLVMConstString + global |
| 1.6 | `NODO_IDENTIFIER` | 5 | ⚠ Bug | No carga variables locales correctamente |
| 1.7 | `NODO_BINARY_OP` | 6 | ✅ Ya estaba | FAdd/FSub/FMul/FDiv/FCmp |

### Bug bloqueante (Fase 1.5 — NUEVO):

El lexer Zen (`tokenizar()` en `lexer.zen`) tiene un bug que hace que solo
genere 6 tokens para `hola.zen` en vez de 220. Esto bloquea todo el pipeline.

**Investigación necesaria**:
- El codegen C++ funciona perfecto (probado con test_argv.zen)
- El lexer Zen compilado inline (616 tokens para un archivo de test) funciona
- Pero cuando ./main ejecuta tokenizar() sobre hola.zen, solo genera 6 tokens

**Hipótesis**: el bug está en cómo `tokenizar()` lee el código desde
`leer_archivo()`. Quizás `leer_archivo` del codegen C++ no retorna el
contenido completo cuando se llama desde el contexto del ./main.

### Validación (cuando el lexer funcione):
```bash
./main ejemplos/hola.zen --run
# Debe imprimir:
# Hola desde Zen!
# 42
# ...
```

### Commit esperado: `Zen sobre Zen: Fase 1 - codegen statement básico + fix lexer`

---

## FASE 1.5: Fix lexer Zen ⏳ NUEVO — BLOCKER

**Objetivo**: que `tokenizar()` en `lexer.zen` genere todos los tokens

### Pasos:

1. Agregar printf de debug en `tokenizar()` para ver qué lee
2. Comparar el IR de `tokenizar()` generado por `./main` vs `./build/zen`
3. Verificar que `leer_archivo()` retorna el contenido completo
4. Verificar que el loop `mientras pos < len` itera correctamente

### Test de validación:
```bash
./main ejemplos/hola.zen --ir
# Debe decir "220 tokens generados" (no "6")
```

---

## FASE 2: Codegen de control flow ⏳ ~2 h (sin cambios)

[...]

## FASE 3: Codegen de features avanzadas ⏳ ~2 h (sin cambios)

[...]

## FASE 4: Bootstrap T-stage 🔥 ⏳ ~2-3 h (sin cambios)

[...]

## FASE 5: Integración del linker ⏳ ~1-2 h (sin cambios)

[...]

## FASE 6: Embeber runtimes Android ⏳ ~1-2 h (sin cambios)

[...]

## FASE 7: Limpieza y release ⏳ ~1 h (sin cambios)

[...]

---

## Resumen actualizado

| Fase | Tiempo | Estado | Notas |
|------|--------|--------|-------|
| 1.5 - Fix lexer | ? | ⏳ BLOCKER | Nuevo — se descubrió en Fase 1 |
| 1 - Codegen básico | 1 h más | 🔄 En progreso | PRINT implementado, falta que el lexer funcione |
| 2 - Control flow | 2 h | ⏳ pendiente | |
| 3 - Features avanzadas | 2 h | ⏳ pendiente | |
| 4 - **Bootstrap T-stage** 🔥 | 2-3 h | ⏳ pendiente | |
| 5 - Linker integrado | 1-2 h | ⏳ pendiente | |
| 6 - Android standalone | 1-2 h | ⏳ pendiente | |
| 7 - Release | 1 h | ⏳ pendiente | |
