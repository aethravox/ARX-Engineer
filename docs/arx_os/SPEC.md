# ARX OS — Specification v1.0

> **Mini-SO propietario** que el ARX Client implementa.
> Todas las apps (.aex) llaman a estas syscalls, no a libc/Win32.
> Sin ARX Client = sin ejecución (los .so nativos crashean al primer call).

---

## 1. Filosofía

ARX OS no es un SO de verdad (no tiene kernel, drivers, scheduler de procesos).
Es una **API de runtime** que el ARX Client expone como símbolos exportados.
Los binarios nativos (.so/.dll) generados por el codegen de Zen llaman a estos
símbolos en lugar de llamar a glibc/msvcrt.

**Propiedades clave:**
- **ABI propietaria**: símbolos `__arx_syscall_*` no existen fuera del cliente.
- **Sandboxed**: cada syscall pasa por el verificador de permisos del manifest.
- **Triple verificación**: cliente verifica .aex, .aex verifica .so, .so verifica .aex.
- **Portátil**: un .aex corre en cualquier OS con cliente ARX (no hay syscalls del OS).

---

## 2. Convención de llamadas

### Símbolo canónico

Toda syscall se expone como:

```c
ARX_API returnType __arx_syscall_<name>(params...);
```

Donde `ARX_API` es:
- En Linux/macOS: atributo default visibility (`__attribute__((visibility("default")))`)
- En Windows: `__declspec(dllexport)`

### Tipos primitivos ARX

Para portabilidad, las syscalls usan tipos fijos:

| Tipo ARX | C type | Tamaño |
|----------|--------|--------|
| `arx_i8` | `int8_t` | 1 |
| `arx_u8` | `uint8_t` | 1 |
| `arx_i16` | `int16_t` | 2 |
| `arx_u16` | `uint16_t` | 2 |
| `arx_i32` | `int32_t` | 4 |
| `arx_u32` | `uint32_t` | 4 |
| `arx_i64` | `int64_t` | 8 |
| `arx_u64` | `uint64_t` | 8 |
| `arx_f32` | `float` | 4 |
| `arx_f64` | `double` | 8 |
| `arx_bool` | `int32_t` | 4 (0=false, 1=true) |
| `arx_handle` | `int32_t` | 4 (resource ID, -1 = invalid) |
| `arx_str` | `const char*` | 8 (UTF-8, null-terminated) |

### Códigos de error

```c
#define ARX_OK              0
#define ARX_ERR_INVALID    -1
#define ARX_ERR_PERM       -2   // sandbox denied
#define ARX_ERR_NOTFOUND   -3
#define ARX_ERR_NOMEM      -4
#define ARX_ERR_BUSY       -5
#define ARX_ERR_IO         -6
#define ARX_ERR_FORMAT     -7
#define ARX_ERR_TAMPERED   -8   // verificacion mutua fallo
#define ARX_ERR_WRONG_SIG  -9   // firma incorrecta
```

---

## 3. Syscalls por categoría

### 3.1 Lifecycle (obligatorias, todo .aex las usa)

```c
// Llamado al cargar el .aex. Retorna 0 = OK, !=0 = refuse to run.
arx_i32 __arx_syscall_init(arx_u64 aex_handle);

// Llamado cada frame (60Hz por defecto). delta en segundos.
void __arx_syscall_update(arx_f32 delta);

// Llamado cada frame para renderizar. Llamado despues de update.
void __arx_syscall_render();

// Llamado al cerrar el .aex.
void __arx_syscall_shutdown();

// Llamado cuando el .aex pierde foco (background).
void __arx_syscall_pause();

// Llamado cuando el .aex recupera foco.
void __arx_syscall_resume();
```

### 3.2 Render (OpenGL backend en el cliente)

```c
// Texturas
arx_handle __arx_syscall_texture_load(arx_str path);  // path dentro del .aex
arx_i32    __arx_syscall_texture_free(arx_handle tex);
arx_i32    __arx_syscall_texture_size(arx_handle tex, arx_i32* w, arx_i32* h);

// Draw
void __arx_syscall_clear(arx_f32 r, arx_f32 g, arx_f32 b, arx_f32 a);
void __arx_syscall_draw_sprite(arx_handle tex, arx_f32 x, arx_f32 y);
void __arx_syscall_draw_sprite_rect(arx_handle tex, arx_f32 sx, arx_f32 sy,
                                      arx_f32 sw, arx_f32 sh,
                                      arx_f32 dx, arx_f32 dy,
                                      arx_f32 dw, arx_f32 dh);
void __arx_syscall_draw_text(arx_str text, arx_f32 x, arx_f32 y, arx_f32 size);
void __arx_syscall_draw_rect(arx_f32 x, arx_f32 y, arx_f32 w, arx_f32 h,
                               arx_f32 r, arx_f32 g, arx_f32 b, arx_f32 a);
void __arx_syscall_draw_circle(arx_f32 x, arx_f32 y, arx_f32 radius,
                                 arx_f32 r, arx_f32 g, arx_f32 b, arx_f32 a);
void __arx_syscall_draw_line(arx_f32 x1, arx_f32 y1, arx_f32 x2, arx_f32 y2,
                               arx_f32 r, arx_f32 g, arx_f32 b, arx_f32 a);

// Viewport / camara
void __arx_syscall_set_viewport(arx_i32 x, arx_i32 y, arx_i32 w, arx_i32 h);
void __arx_syscall_camera_2d(arx_f32 x, arx_f32 y, arx_f32 zoom, arx_f32 rotation);
```

### 3.3 Audio

```c
arx_handle __arx_syscall_sound_load(arx_str path);
arx_i32    __arx_syscall_sound_play(arx_handle snd, arx_f32 volume);
arx_i32    __arx_syscall_sound_pause(arx_handle snd);
arx_i32    __arx_syscall_sound_stop(arx_handle snd);
arx_i32    __arx_syscall_music_play(arx_handle music, arx_f32 volume);
void       __arx_syscall_music_stop();
```

### 3.4 Input

```c
arx_bool __arx_syscall_key_pressed(arx_i32 keycode);    // keycode = ARX_KEY_*
arx_bool __arx_syscall_key_just_pressed(arx_i32 keycode);
arx_bool __arx_syscall_mouse_down(arx_i32 button);      // 0=left, 1=right, 2=middle
void     __arx_syscall_mouse_pos(arx_f32* x, arx_f32* y);
arx_bool __arx_syscall_action_pressed(arx_str name);    // input map configurable
```

### 3.5 Filesystem (solo sandbox)

```c
// Lee un archivo del .aex (assets empaquetados). Read-only.
arx_i32 __arx_syscall_res_read(arx_str path, arx_u8* buf, arx_i32 size);
arx_i32 __arx_syscall_res_size(arx_str path);

// Lee/escribe en el sandbox del paquete (sandbox:/<package_name>/)
arx_handle __arx_syscall_fs_open(arx_str path, arx_i32 mode);  // 0=read, 1=write, 2=append
arx_i32    __arx_syscall_fs_read(arx_handle fh, arx_u8* buf, arx_i32 size);
arx_i32    __arx_syscall_fs_write(arx_handle fh, const arx_u8* buf, arx_i32 size);
arx_i32    __arx_syscall_fs_close(arx_handle fh);
arx_i32    __arx_syscall_fs_exists(arx_str path);
arx_i32    __arx_syscall_fs_delete(arx_str path);
```

### 3.6 Network (con sandbox de endpoints)

```c
// Conecta a host:port. Verifica contra el manifest antes de conectar.
arx_handle __arx_syscall_net_connect(arx_str host, arx_i32 port);
arx_i32    __arx_syscall_net_send(arx_handle sock, const arx_u8* buf, arx_i32 size);
arx_i32    __arx_syscall_net_recv(arx_handle sock, arx_u8* buf, arx_i32 size);
arx_i32    __arx_syscall_net_close(arx_handle sock);

// Server side (si el manifest lo permite)
arx_handle __arx_syscall_net_listen(arx_i32 port);
arx_handle __arx_syscall_net_accept(arx_handle server);

// HTTP client (si el manifest permite los endpoints)
arx_i32 __arx_syscall_http_get(arx_str url, arx_u8* buf, arx_i32 size);
```

### 3.7 Time

```c
arx_f32  __arx_syscall_time_delta();
arx_u64  __arx_syscall_time_now_ms();
arx_u64  __arx_syscall_time_now_us();
void     __arx_syscall_sleep_ms(arx_u32 ms);
arx_i32  __arx_syscall_fps();
```

### 3.8 Memory (custom allocator con tracking)

```c
arx_u8* __arx_syscall_alloc(arx_u64 size);
arx_u8* __arx_syscall_alloc_aligned(arx_u64 size, arx_u32 alignment);
void    __arx_syscall_free(void* ptr);
arx_u64 __arx_syscall_mem_used();
arx_u64 __arx_syscall_mem_peak();
```

### 3.9 Logging

```c
void __arx_syscall_log(arx_str message);
void __arx_syscall_log_warn(arx_str message);
void __arx_syscall_log_error(arx_str message);
```

### 3.10 Verificación mutua (la pieza clave)

```c
// Devuelve el .aex completo en memoria (read-only para el .so).
// El .so lo usa para verificar la firma y comparar el hash.
arx_i32 __arx_syscall_get_aex_bytes(const arx_u8** out_data, arx_u64* out_size);

// Devuelve la pubkey del creador que el cliente valido (32 bytes Ed25519).
arx_i32 __arx_syscall_get_creator_pubkey(arx_u8 out_pubkey[32]);

// Devuelve el manifest JSON parseado.
arx_i32 __arx_syscall_get_manifest_json(const char** out_json);
```

### 3.11 Sistema (info del cliente)

```c
arx_i32 __arx_syscall_get_platform();      // 0=linux, 1=windows, 2=macos, 3=android, 4=web
arx_i32 __arx_syscall_get_screen_size(arx_i32* w, arx_i32* h);
arx_i32 __arx_syscall_get_language();      // 0=es, 1=en, ...
arx_str  __arx_syscall_get_version();      // version del cliente ARX
```

---

## 4. Keycodes ARX (independientes del OS)

```c
#define ARX_KEY_UNKNOWN   0
#define ARX_KEY_A         1
#define ARX_KEY_B         2
// ...
#define ARX_KEY_Z         26
#define ARX_KEY_0         27
#define ARX_KEY_1         28
// ...
#define ARX_KEY_9         36
#define ARX_KEY_SPACE     37
#define ARX_KEY_ENTER     38
#define ARX_KEY_ESCAPE    39
#define ARX_KEY_BACKSPACE 40
#define ARX_KEY_TAB       41
#define ARX_KEY_UP        100
#define ARX_KEY_DOWN      101
#define ARX_KEY_LEFT      102
#define ARX_KEY_RIGHT     103
#define ARX_KEY_SHIFT     104
#define ARX_KEY_CTRL      105
#define ARX_KEY_ALT       106
#define ARX_KEY_F1        200
// ...
#define ARX_KEY_F12       211
#define ARX_MOUSE_LEFT    300
#define ARX_MOUSE_RIGHT   301
#define ARX_MOUSE_MIDDLE  302
```

---

## 5. Formato `.so` ARX (ELF con binding embebido)

El `.so` generado por el codegen de Zen con flag `--arx-os` tiene:

### 5.1 Entry point

No tiene `main()`. En su lugar exporta:

```c
arx_i32 arx_main();           // llamada por el cliente tras dlopen
arx_str arx_get_version();    // version del .so
```

### 5.2 Sección `.arx_binding`

Sección ELF custom con la estructura:

```c
#pragma pack(push, 1)
struct ArxSoBinding {
    arx_u32 magic;            // 0x42524158 = "ARXB"
    arx_u32 version;          // 1
    arx_u8  creator_pubkey[32];   // pubkey Ed25519 del creador original
    arx_u8  expected_aex_hash[32]; // SHA-256 del .aex (sin contar el binding)
    arx_u8  self_hash[32];        // SHA-256 del .so (sin contar el binding)
    arx_u64 compile_time;         // timestamp Unix
    arx_u32 flags;                // bit 0: obfuscated, bit 1: AOT
};
#pragma pack(pop)

extern const ArxSoBinding __arx_binding;  // generada por el linker
```

### 5.3 Flujo de verificación mutua (en runtime)

Dentro de `arx_main()` (generado por el codegen):

```c
arx_i32 arx_main() {
    // 1. Pedir al cliente los bytes del .aex
    const arx_u8* aex_bytes;
    arx_u64 aex_size;
    __arx_syscall_get_aex_bytes(&aex_bytes, &aex_size);
    
    // 2. Verificar hash del .aex
    arx_u8 actual_hash[32];
    arx_sha256(aex_bytes, aex_size, actual_hash);
    if (memcmp(actual_hash, __arx_binding.expected_aex_hash, 32) != 0) {
        __arx_syscall_log_error("ARX: .aex tampered (hash mismatch)");
        return ARX_ERR_TAMPERED;
    }
    
    // 3. Verificar firma Ed25519
    arx_u8 client_pubkey[32];
    __arx_syscall_get_creator_pubkey(client_pubkey);
    if (memcmp(client_pubkey, __arx_binding.creator_pubkey, 32) != 0) {
        __arx_syscall_log_error("ARX: creator mismatch");
        return ARX_ERR_WRONG_SIG;
    }
    
    // 4. Todo OK → ejecutar juego
    return arx_game_run();
}
```

---

## 6. Formato `.zbc` — Zen Bytecode (para VM, no nativo)

Alternativa al .so nativo: bytecode portable interpretado por la VM Zen del cliente.

### 6.1 Cabecera

```c
#pragma pack(push, 1)
struct ZbcHeader {
    arx_u32 magic;            // 0x5A424331 = "ZBC1"
    arx_u16 version;          // 1
    arx_u16 flags;            // bit 0: debug info
    arx_u64 created_unix;
    arx_u32 string_table_offset;
    arx_u32 string_table_size;
    arx_u32 function_table_offset;
    arx_u32 function_table_size;
    arx_u32 code_offset;
    arx_u32 code_size;
    arx_u32 debug_offset;     // 0 si no hay
    arx_u32 debug_size;
};
#pragma pack(pop)
```

### 6.2 Bytecode opcodes (stack-based, ~50 opcodes)

```
OP_LOAD_CONST     0x01  // push const from table
OP_LOAD_LOCAL     0x02  // push local var
OP_STORE_LOCAL    0x03  // pop and store to local
OP_LOAD_GLOBAL    0x04
OP_STORE_GLOBAL   0x05
OP_ADD            0x10
OP_SUB            0x11
OP_MUL            0x12
OP_DIV            0x13
OP_MOD            0x14
OP_NEG            0x15
OP_EQ             0x20
OP_NEQ            0x21
OP_LT             0x22
OP_GT             0x23
OP_LE             0x24
OP_GE             0x25
OP_AND            0x26
OP_OR             0x27
OP_NOT            0x28
OP_JUMP           0x30  // unconditional
OP_JUMP_IF_FALSE  0x31  // pop, jump if false
OP_CALL           0x40  // call function
OP_CALL_NATIVE    0x41  // call __arx_syscall_*
OP_RETURN         0x42
OP_POP            0x50
OP_DUP            0x51
OP_NEW_LIST       0x60
OP_NEW_STRUCT     0x61
OP_INDEX_GET      0x62
OP_INDEX_SET      0x63
OP_MEMBER_GET     0x64
OP_MEMBER_SET     0x65
OP_PRINT          0x70  // llama a __arx_syscall_log
```

---

## 7. Manifest extendido (con backend)

```json
{
  "scripts": {
    "language": "zen",
    "entry": "main.so",
    "type": "arx_native_so_v1",
    "binding_expected": true
  }
}
```

O para bytecode:

```json
{
  "scripts": {
    "language": "zen",
    "entry": "main.zbc",
    "type": "arx_zen_bytecode_v1",
    "binding_expected": false
  }
}
```

O para Luau:

```json
{
  "scripts": {
    "language": "luau",
    "entry": "main.luau",
    "type": "arx_luau_bytecode_v3",
    "binding_expected": false
  }
}
```

El cliente lee `scripts.type` y carga el backend correspondiente.

---

## 8. Roadmap de implementación

### Fase 1 — Esqueleto del ARX OS (3 días)
- [ ] `client/src/kernel/arx_os.h` — todas las syscalls declaradas
- [ ] `client/src/kernel/syscall_render.c` — stubs (imprime a log)
- [ ] `client/src/kernel/syscall_lifecycle.c` — init/update/render/shutdown
- [ ] `client/src/kernel/syscall_verify.c` — verificación mutua
- [ ] Test: .aex con .so stub que llame `__arx_syscall_log("hola")`

### Fase 2 — Codegen de Zen con `--arx-os` (1 semana)
- [ ] Modificar `engine/zen/src/codegen.cpp` para emitir `__arx_syscall_*`
- [ ] Generar `arx_main()` con verificación mutua embebida
- [ ] Generar sección `.arx_binding` con hash + pubkey
- [ ] Test: compilar `.zen` simple → `.so` que NO corre en bash pero SÍ en cliente

### Fase 3 — Renderer OpenGL en el cliente (1 semana)
- [ ] Implementar `syscall_render.c` con OpenGL 3.3
- [ ] Texturas (stb_image), sprites, text (FreeType)
- [ ] Test: .zen que dibuje un sprite en pantalla

### Fase 4 — VM Zen (bytecode .zbc) (1 semana)
- [ ] Bytecode emitter en `engine/zen/src/bytecode.cpp`
- [ ] VM en `client/src/runtime/zen_vm.c` (stack-based, ~1500 líneas)
- [ ] Test: mismo .zen exportado como .zbc, ejecutar en cliente

### Fase 5 — Backend Luau (3 días)
- [ ] `client/src/runtime/luau_runtime.c` — wrapper sobre libluauvm.a
- [ ] Registrar syscalls como funciones Luau globales
- [ ] Test: .luau que dibuje un sprite en pantalla

### Fase 6 — Backend nativo completo (3 días)
- [ ] `client/src/runtime/native_runtime.c` — dlopen + memfd_create
- [ ] Verificación de binding embebido antes de dlopen
- [ ] Test: .so ARX nativo en .aex, ejecutar en cliente

---

## 9. Seguridad — modelo completo

```
┌─ Capa 1: Firma Ed25519 del .aex (creador → usuario)
│  └─ Cliente verifica antes de cargar
│
├─ Capa 2: Hash del .so dentro del .aex (binding embebido)
│  └─ Cliente verifica que el .so no fue reemplazado
│
├─ Capa 3: Hash del .aex dentro del .so (verificación mutua)
│  └─ .so verifica que el .aex no fue modificado
│
├─ Capa 4: Firma Ed25519 verificada por el .so
│  └─ .so verifica que el .aex fue firmado por su creador original
│
├─ Capa 5: ABI propietaria (__arx_syscall_*)
│  └─ Sin cliente, el .so crashea al primer call
│
└─ Capa 6: Sandbox de permisos (manifest)
   └─ Cada syscall pasa por el verificador
```

**Sin la clave privada del creador, es criptográficamente imposible ejecutar
un .aex modificado o extraer el .so para uso fuera de ARX.**
