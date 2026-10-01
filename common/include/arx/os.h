// arx/os.h — ARX OS Syscalls API
//
// Esta es la API que el ARX Client expone a los .aex.
// Los .so nativos generados por Zen con --arx-os llaman a estos símbolos.
// Sin ARX Client, los .so crashean al primer call (símbolos no resueltos).
//
// Ver docs/arx_os/SPEC.md para la especificación completa.

#ifndef ARX_OS_H
#define ARX_OS_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================
// Tipos primitivos ARX
// ============================================================

typedef int8_t   arx_i8;
typedef uint8_t  arx_u8;
typedef int16_t  arx_i16;
typedef uint16_t arx_u16;
typedef int32_t  arx_i32;
typedef uint32_t arx_u32;
typedef int64_t  arx_i64;
typedef uint64_t arx_u64;
typedef float    arx_f32;
typedef double   arx_f64;
typedef int32_t  arx_bool;
typedef int32_t  arx_handle;
typedef const char* arx_str;

#define ARX_NULL_HANDLE (-1)

// ============================================================
// Forward declaration de AexPackage (definido en arx/format/aex.h)
// ============================================================
struct AexPackage;

// ============================================================
// Runtime API (cliente)
// ============================================================

// Inicializa el runtime ARX con un .aex ya abierto y verificado.
// Configura sandbox, guarda bytes del .aex para verificación mutua.
// Retorna ARX_OK o código de error.
arx_i32 arx_runtime_init(struct AexPackage* pkg);

// Libera el runtime.
void arx_runtime_shutdown();

// ============================================================
// Códigos de error
// ============================================================

#define ARX_OK              0
#define ARX_ERR_INVALID    -1
#define ARX_ERR_PERM       -2
#define ARX_ERR_NOTFOUND   -3
#define ARX_ERR_NOMEM      -4
#define ARX_ERR_BUSY       -5
#define ARX_ERR_IO         -6
#define ARX_ERR_FORMAT     -7
#define ARX_ERR_TAMPERED   -8
#define ARX_ERR_WRONG_SIG  -9

// ============================================================
// Visibilidad de símbolos
// ============================================================

#if defined(_WIN32) || defined(_WIN64)
    #define ARX_API __declspec(dllexport)
#else
    #define ARX_API __attribute__((visibility("default")))
#endif

// ============================================================
// 1. Lifecycle
// ============================================================

ARX_API arx_i32 __arx_syscall_init(arx_u64 aex_handle);
ARX_API void    __arx_syscall_update(arx_f32 delta);
ARX_API void    __arx_syscall_render();
ARX_API void    __arx_syscall_shutdown();
ARX_API void    __arx_syscall_pause();
ARX_API void    __arx_syscall_resume();

// ============================================================
// 2. Render
// ============================================================

ARX_API arx_handle __arx_syscall_texture_load(arx_str path);
ARX_API arx_i32    __arx_syscall_texture_free(arx_handle tex);
ARX_API arx_i32    __arx_syscall_texture_size(arx_handle tex, arx_i32* w, arx_i32* h);

ARX_API void __arx_syscall_clear(arx_f32 r, arx_f32 g, arx_f32 b, arx_f32 a);
ARX_API void __arx_syscall_draw_sprite(arx_handle tex, arx_f32 x, arx_f32 y);
ARX_API void __arx_syscall_draw_sprite_rect(arx_handle tex,
                                              arx_f32 sx, arx_f32 sy, arx_f32 sw, arx_f32 sh,
                                              arx_f32 dx, arx_f32 dy, arx_f32 dw, arx_f32 dh);
ARX_API void __arx_syscall_draw_text(arx_str text, arx_f32 x, arx_f32 y, arx_f32 size);
ARX_API void __arx_syscall_draw_rect(arx_f32 x, arx_f32 y, arx_f32 w, arx_f32 h,
                                       arx_f32 r, arx_f32 g, arx_f32 b, arx_f32 a);
ARX_API void __arx_syscall_draw_circle(arx_f32 x, arx_f32 y, arx_f32 radius,
                                         arx_f32 r, arx_f32 g, arx_f32 b, arx_f32 a);
ARX_API void __arx_syscall_draw_line(arx_f32 x1, arx_f32 y1, arx_f32 x2, arx_f32 y2,
                                       arx_f32 r, arx_f32 g, arx_f32 b, arx_f32 a);

ARX_API void __arx_syscall_set_viewport(arx_i32 x, arx_i32 y, arx_i32 w, arx_i32 h);
ARX_API void __arx_syscall_camera_2d(arx_f32 x, arx_f32 y, arx_f32 zoom, arx_f32 rotation);

// ============================================================
// 3. Audio
// ============================================================

ARX_API arx_handle __arx_syscall_sound_load(arx_str path);
ARX_API arx_i32    __arx_syscall_sound_play(arx_handle snd, arx_f32 volume);
ARX_API arx_i32    __arx_syscall_sound_pause(arx_handle snd);
ARX_API arx_i32    __arx_syscall_sound_stop(arx_handle snd);
ARX_API arx_i32    __arx_syscall_music_play(arx_handle music, arx_f32 volume);
ARX_API void       __arx_syscall_music_stop();

// ============================================================
// 4. Input
// ============================================================

ARX_API arx_bool __arx_syscall_key_pressed(arx_i32 keycode);
ARX_API arx_bool __arx_syscall_key_just_pressed(arx_i32 keycode);
ARX_API arx_bool __arx_syscall_mouse_down(arx_i32 button);
ARX_API void     __arx_syscall_mouse_pos(arx_f32* x, arx_f32* y);
ARX_API arx_bool __arx_syscall_action_pressed(arx_str name);

// ============================================================
// 5. Filesystem (sandbox)
// ============================================================

ARX_API arx_i32 __arx_syscall_res_read(arx_str path, arx_u8* buf, arx_i32 size);
ARX_API arx_i32 __arx_syscall_res_size(arx_str path);

ARX_API arx_handle __arx_syscall_fs_open(arx_str path, arx_i32 mode);
ARX_API arx_i32    __arx_syscall_fs_read(arx_handle fh, arx_u8* buf, arx_i32 size);
ARX_API arx_i32    __arx_syscall_fs_write(arx_handle fh, const arx_u8* buf, arx_i32 size);
ARX_API arx_i32    __arx_syscall_fs_close(arx_handle fh);
ARX_API arx_i32    __arx_syscall_fs_exists(arx_str path);
ARX_API arx_i32    __arx_syscall_fs_delete(arx_str path);

// ============================================================
// 6. Network (con sandbox de endpoints)
// ============================================================

ARX_API arx_handle __arx_syscall_net_connect(arx_str host, arx_i32 port);
ARX_API arx_i32    __arx_syscall_net_send(arx_handle sock, const arx_u8* buf, arx_i32 size);
ARX_API arx_i32    __arx_syscall_net_recv(arx_handle sock, arx_u8* buf, arx_i32 size);
ARX_API arx_i32    __arx_syscall_net_close(arx_handle sock);

ARX_API arx_handle __arx_syscall_net_listen(arx_i32 port);
ARX_API arx_handle __arx_syscall_net_accept(arx_handle server);

ARX_API arx_i32 __arx_syscall_http_get(arx_str url, arx_u8* buf, arx_i32 size);

// ============================================================
// 7. Time
// ============================================================

ARX_API arx_f32 __arx_syscall_time_delta();
ARX_API arx_u64 __arx_syscall_time_now_ms();
ARX_API arx_u64 __arx_syscall_time_now_us();
ARX_API void    __arx_syscall_sleep_ms(arx_u32 ms);
ARX_API arx_i32 __arx_syscall_fps();

// ============================================================
// 8. Memory (custom allocator)
// ============================================================

ARX_API arx_u8* __arx_syscall_alloc(arx_u64 size);
ARX_API arx_u8* __arx_syscall_alloc_aligned(arx_u64 size, arx_u32 alignment);
ARX_API void    __arx_syscall_free(void* ptr);
ARX_API arx_u64 __arx_syscall_mem_used();
ARX_API arx_u64 __arx_syscall_mem_peak();

// ============================================================
// 9. Logging
// ============================================================

ARX_API void __arx_syscall_log(arx_str message);
ARX_API void __arx_syscall_log_warn(arx_str message);
ARX_API void __arx_syscall_log_error(arx_str message);

// ============================================================
// 10. Verificación mutua (la pieza clave)
// ============================================================

ARX_API arx_i32 __arx_syscall_get_aex_bytes(const arx_u8** out_data, arx_u64* out_size);
ARX_API arx_i32 __arx_syscall_get_creator_pubkey(arx_u8 out_pubkey[32]);
ARX_API arx_i32 __arx_syscall_get_manifest_json(const char** out_json);

// ============================================================
// 11. Sistema
// ============================================================

ARX_API arx_i32 __arx_syscall_get_platform();
ARX_API arx_i32 __arx_syscall_get_screen_size(arx_i32* w, arx_i32* h);
ARX_API arx_i32 __arx_syscall_get_language();
ARX_API arx_str  __arx_syscall_get_version();

// ============================================================
// 12. Memory / String helpers (para reemplazar libc en .so ARX)
// ============================================================

// Estas syscalls existen para que el codegen de Zen pueda generar .so
// que NO dependan de libc. El runtime de Zen (zen_concat, __zen_num_to_str,
// __zen_list_*) llama a estas en lugar de malloc/free/memcpy/strlen/strcmp.

ARX_API void* __arx_syscall_mem_alloc(arx_u64 size);
ARX_API void  __arx_syscall_mem_free(void* ptr);
ARX_API void  __arx_syscall_mem_copy(void* dst, const void* src, arx_u64 size);
ARX_API arx_u64 __arx_syscall_str_len(const char* s);
ARX_API arx_i32 __arx_syscall_str_cmp(const char* a, const char* b);
ARX_API char* __arx_syscall_str_dup(const char* s);
ARX_API arx_i32 __arx_syscall_str_format_int(char* buf, arx_u64 size, arx_i64 value);
ARX_API arx_i32 __arx_syscall_str_format_float(char* buf, arx_u64 size, arx_f64 value);

// ============================================================
// 13. Binding embebido (verificación mutua .so ↔ .aex)
// ============================================================

// El .so ARX nativo lleva embebido un ArxSoBinding con:
//   - creator_pubkey (32 bytes Ed25519)
//   - expected_aex_hash (32 bytes SHA-256 del .aex)
//   - self_hash (32 bytes SHA-256 del .so sin el binding)
//
// Al arrancar, arx_main() llama a __arx_syscall_verify_binding:
//   1. El cliente compara el binding embebido con el .aex que cargó
//   2. Si el hash del .aex no coincide → ARX_ERR_TAMPERED
//   3. Si la pubkey no coincide → ARX_ERR_WRONG_SIG
//   4. Si todo OK → ARX_OK y el .so puede ejecutar
//
// binding_ptr apunta al ArxSoBinding embebido en el .so.
ARX_API arx_i32 __arx_syscall_verify_binding(const void* binding_ptr);

// ============================================================
// Keycodes
// ============================================================

#define ARX_KEY_UNKNOWN    0
#define ARX_KEY_A          1
#define ARX_KEY_B          2
#define ARX_KEY_C          3
#define ARX_KEY_D          4
#define ARX_KEY_E          5
#define ARX_KEY_F          6
#define ARX_KEY_G          7
#define ARX_KEY_H          8
#define ARX_KEY_I          9
#define ARX_KEY_J          10
#define ARX_KEY_K          11
#define ARX_KEY_L          12
#define ARX_KEY_M          13
#define ARX_KEY_N          14
#define ARX_KEY_O          15
#define ARX_KEY_P          16
#define ARX_KEY_Q          17
#define ARX_KEY_R          18
#define ARX_KEY_S          19
#define ARX_KEY_T          20
#define ARX_KEY_U          21
#define ARX_KEY_V          22
#define ARX_KEY_W          23
#define ARX_KEY_X          24
#define ARX_KEY_Y          25
#define ARX_KEY_Z          26
#define ARX_KEY_0          27
#define ARX_KEY_1          28
#define ARX_KEY_2          29
#define ARX_KEY_3          30
#define ARX_KEY_4          31
#define ARX_KEY_5          32
#define ARX_KEY_6          33
#define ARX_KEY_7          34
#define ARX_KEY_8          35
#define ARX_KEY_9          36
#define ARX_KEY_SPACE      37
#define ARX_KEY_ENTER      38
#define ARX_KEY_ESCAPE     39
#define ARX_KEY_BACKSPACE  40
#define ARX_KEY_TAB        41
#define ARX_KEY_UP         100
#define ARX_KEY_DOWN       101
#define ARX_KEY_LEFT       102
#define ARX_KEY_RIGHT      103
#define ARX_KEY_SHIFT      104
#define ARX_KEY_CTRL       105
#define ARX_KEY_ALT        106
#define ARX_KEY_F1         200
#define ARX_KEY_F2         201
#define ARX_KEY_F3         202
#define ARX_KEY_F4         203
#define ARX_KEY_F5         204
#define ARX_KEY_F6         205
#define ARX_KEY_F7         206
#define ARX_KEY_F8         207
#define ARX_KEY_F9         208
#define ARX_KEY_F10        209
#define ARX_KEY_F11        210
#define ARX_KEY_F12        211
#define ARX_MOUSE_LEFT     300
#define ARX_MOUSE_RIGHT    301
#define ARX_MOUSE_MIDDLE   302

// ============================================================
// Binding embebido en .so ARX nativo
// ============================================================

#pragma pack(push, 1)
typedef struct {
    arx_u32 magic;                  // 0x42524158 = "ARXB"
    arx_u32 version;                // 1
    arx_u8  creator_pubkey[32];     // Ed25519 pubkey del creador
    arx_u8  expected_aex_hash[32];  // SHA-256 del .aex
    arx_u8  self_hash[32];          // SHA-256 del .so (sin este binding)
    arx_u64 compile_time;           // timestamp Unix
    arx_u32 flags;                  // bit 0: obfuscated, bit 1: AOT
} ArxSoBinding;
#pragma pack(pop)

#define ARX_BINDING_MAGIC   0x42524158u   // "ARXB" little-endian
#define ARX_BINDING_VERSION 1u

// El .so ARX nativo debe exportar este símbolo.
// El codegen de Zen lo genera al compilar con --arx-os.
// extern const ArxSoBinding __arx_binding;

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ARX_OS_H
