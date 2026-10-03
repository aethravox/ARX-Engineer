# Zen Self-Hosting Roadmap

Progreso del bootstrap de Zen sobre Zen — el compilador Zen escrito en Zen.

## Estado actual (commit `db3f0fe`)

```
✅ Modularización completa (6 archivos .zen con imports)
✅ Compila a binario nativo con LLVM embebido
✅ ./main -h muestra ayuda
✅ ./main archivo.zen lee y procesa sin crashear
✅ Pipeline completo: leer → tokenizar → parsear → codegen → .ll
✅ Exit code 0 limpio
❌ codegen_statement no genera IR para los nodos (solo retorna 0)
```

---

## FASE 1: Codegen de statements básicos ⏳ ~2-3 h

**Objetivo**: que `./main hola.zen --run` imprima "Hola desde Zen!"

### Casos a implementar en `codegen.zen` → `codegen_statement()`:

| # | Nodo | Tipo | Qué hacer |
|---|------|------|-----------|
| 1.1 | `NODO_PRINT` | 12 | Generar `printf("%g\n", arg)` para números, `printf("%s\n", arg)` para strings |
| 1.2 | `NODO_ASSIGN` | 10 | `LLVMBuildAlloca` + `LLVMBuildStore` para variables locales |
| 1.3 | `NODO_EXPR_STMT` | 22 | Evaluar expresión y descartar resultado |
| 1.4 | `NODO_NUMBER` | 1 | `LLVMConstReal(double, valor)` — ya está |
| 1.5 | `NODO_STRING` | 2 | `LLVMConstString` + global — ya está |
| 1.6 | `NODO_IDENTIFIER` | 5 | Load de variable local/global |
| 1.7 | `NODO_BINARY_OP` | 6 | FAdd/FSub/FMul/FDiv/FCmp — ya está |

### Validación:
```bash
./main ejemplos/hola.zen --run
# Debe imprimir:
# Hola desde Zen!
# 42
# 3.14159
# ... etc
```

### Commit esperado: `Zen sobre Zen: Fase 1 - codegen statement básico (Print, Assign, Expr)`

---

## FASE 2: Codegen de control flow ⏳ ~2 h

**Objetivo**: que `hola.zen` con bucles y condicionales funcione

### Casos a implementar:

| # | Nodo | Tipo | Qué hacer |
|---|------|------|-----------|
| 2.1 | `NODO_IF` | 13 | `LLVMBuildCondBr` + 3 basic blocks (then, else, merge) |
| 2.2 | `NODO_WHILE` | 16 | Loop con 3 basic blocks (cond, body, end) |
| 2.3 | `NODO_FOR_RANGE` | 14 | Loop con alloca para var + 3 basic blocks |
| 2.4 | `NODO_FUNC_DECL` | 20 | `LLVMAddFunction` + entry block |
| 2.5 | `NODO_RETURN` | 21 | `LLVMBuildRet` |

### Validación:
```bash
./main ejemplos/hola.zen --run
# Debe imprimir TODO el output de hola.zen correctamente
# incluyendo bucles "para", "mientras", condicionales "si"
```

### Commit esperado: `Zen sobre Zen: Fase 2 - control flow (if, while, for, func, return)`

---

## FASE 3: Codegen de features avanzadas ⏳ ~2 h

**Objetivo**: que TODOS los ejemplos corran con `./main`

### Casos a implementar:

| # | Nodo | Tipo | Qué hacer |
|---|------|------|-----------|
| 3.1 | `NODO_FUNC_CALL` (user) | 9 | `LLVMBuildCall` para funciones user-defined |
| 3.2 | `NODO_LIST_LIT` | 23 | `__zen_list_create` + `__zen_list_push` |
| 3.3 | `NODO_INDEX_ACCESS` | 24 | `__zen_list_get` con conversión i8*→i64 |
| 3.4 | `NODO_STRUCT_LIT` | 27 | `__zen_struct_create` + `__zen_struct_set` |
| 3.5 | `NODO_MEMBER_ACCESS` | 28 | `__zen_struct_get` |
| 3.6 | `NODO_BREAK` | 18 | `LLVMBuildBr` al end_bb del loop |
| 3.7 | `NODO_CONTINUE` | 19 | `LLVMBuildBr` al cond_bb del loop |
| 3.8 | `NODO_TERNARY` | 8 | `LLVMBuildSelect` |

### Validación:
```bash
# Probar cada ejemplo:
for f in ejemplos/*.zen; do
    echo "=== $f ==="
    ./main "$f" --run
done
# Todos deben funcionar igual que con ./build/zen (C++)
```

### Commit esperado: `Zen sobre Zen: Fase 3 - features avanzadas (listas, structs, break, continue)`

---

## FASE 4: Bootstrap T-stage 🔥 ⏳ ~2-3 h

**Objetivo**: que `./main` compile `main.zen` a un binario funcional

### Pasos:

1. **Stage 2**: `./main src/main.zen --obj` → genera `main2.o`
2. **Link**: `cc main2.o -o main2 $(llvm-config-14 --libs ...) -lm`
3. **Stage 3**: `./main2 src/main.zen --obj` → genera `main3.o`
4. **Validación**: `diff main2 main3` deben ser idénticos (o equivalentes)

### Validación:
```bash
./main src/main.zen --obj
cc main2.o -o main2 $(llvm-config-14 --libs core executionengine native) \
              $(llvm-config-14 --system-libs) -lm -lstdc++ -lpthread

./main2 ejemplos/hola.zen --run
# Debe imprimir lo mismo que ./main

# Bootstrap cerrado:
./main2 src/main.zen --obj
cc main3.o -o main3 ...
diff <(./main --version) <(./main2 --version)  # deben ser iguales
```

### Commit esperado: `🔥 Zen sobre Zen: BOOTSTRAP T-STAGE COMPLETO - Self-hosting logrado`

---

## FASE 5: Integración del linker ⏳ ~1-2 h

**Objetivo**: que `./main --build` use `zen_linker` (embebido) en vez de `cc`

### Cambios:

1. En `main.zen`, reemplazar `system("cc " + obj + " -o " + exe + " -lm")` 
   por FFI a `link_object()` de `zen_linker.cpp`
2. Probar cross-compile:
   - `./main hola.zen --build --platform linux` → binario Linux
   - `./main hola.zen --build --platform windows` → `.exe` sin MinGW
   - `./main hola.zen --build --platform android` → binario Android

### Commit esperado: `Zen sobre Zen: Fase 5 - linker embebido integrado (cross-compile)`

---

## FASE 6: Embeber runtimes Android ⏳ ~1-2 h

**Objetivo**: que `./main --platform android` no necesite NDK

### Pasos:

1. Extraer crt objects del NDK (4 arquitecturas × 4 crt = 64 KB):
   ```
   runtimes/android/
   ├── aarch64/    (crtbegin_dynamic.o, crtend_android.o, ...)
   ├── arm/        (idem)
   ├── x86_64/     (idem)
   └── i686/       (idem)
   ```
2. Embeber en `zen_runtimes.cpp` como arrays de bytes
3. Modificar `zen_linker.cpp` caso `android` para usar `ld.lld-14` + crt embebidos + Bionic dinámica

### Commit esperado: `Zen sobre Zen: Fase 6 - Android standalone (sin NDK requerido)`

---

## FASE 7: Limpieza y release ⏳ ~1 h

**Objetivo**: dejar el código production-ready

### Tareas:

1. Borrar `src_Old/` (ya no se necesita — todo en Zen)
2. Borrar `zen_compiler.zen.bak`
3. Actualizar README principal del repositorio
4. Actualizar `engine/zen/README.md` con instrucciones nuevas
5. Tag: `v2.0-zen-self-hosted`

### Commit esperado: `Zen v2.0 - Self-hosted release 🎉`

---

## Resumen

| Fase | Tiempo | Estado | Desbloquea |
|------|--------|--------|------------|
| 1 - Codegen básico | 2-3 h | ⏳ pendiente | Fase 2 |
| 2 - Control flow | 2 h | ⏳ pendiente | Fase 3 |
| 3 - Features avanzadas | 2 h | ⏳ pendiente | Fase 4 |
| 4 - **Bootstrap T-stage** 🔥 | 2-3 h | ⏳ pendiente | Fase 5 |
| 5 - Linker integrado | 1-2 h | ⏳ pendiente | Fase 6 |
| 6 - Android standalone | 1-2 h | ⏳ pendiente | Fase 7 |
| 7 - Release | 1 h | ⏳ pendiente | — |

**Total estimado**: 11-15 horas (~3-5 sesiones más)

---

## Cómo retomar cada sesión

Cada sesión, el agente debe:

1. **Conectar por SSH** al PC (148.101.196.52, usuario aethravox, key en `/home/z/my-project/scripts/arx_key`)
2. **Leer este roadmap** (`/home/z/my-project/zen_src/ROADMAP.md`)
3. **Hacer `git pull`** en el repo para tener lo último
4. **Completar la fase siguiente** del roadmap
5. **Commit + push** al terminar
6. **Actualizar este roadmap** marcando la fase como ✅

### Comando para inicializar una sesión:

```bash
# En el agente:
cd /home/z/my-project/scripts
python3 -c "
import paramiko
client = paramiko.SSHClient()
client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
client.connect('148.101.196.52', port=22, username='aethravox',
              key_filename='/home/z/my-project/scripts/arx_key', timeout=15)
# Verificar último commit
_, out, _ = client.exec_command('cd ~/Escritorio/Projectos/Motores/ARX && git log -3 --oneline')
print(out.read().decode())
"
```

Luego leer este archivo y continuar con la siguiente fase.

---

## Notas importantes

- **NO borrar `src_Old/`** hasta Fase 7 — es el palo de control para validar que el codegen Zen produce el mismo IR que el C++
- **Cada fase debe terminar con commit + push** — nunca dejar trabajo sin commitear
- **Si una fase se atasca** > 1 hora, parar y volver a empezar con cabeza fría
- **Bug más común**: el codegen C++ no convierte `i8*` (string boxed) a `double`/`i64` cuando se pasa a funciones C. Si hay segfault, mirar el IR generado y verificar conversiones
