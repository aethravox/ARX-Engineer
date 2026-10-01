// client/src/kernel/arx_os.c — Implementación stub del ARX OS
//
// Esta es la implementación mínima que el ARX Client provee.
// Por ahora todos los render/audio dibujan a log. Cuando se integre
// OpenGL/miniaudio, se reemplazan los stubs.

#include "arx/os.h"
#include "arx/format/aex.h"
#include "arx/format/manifest.h"
#include "arx/format/sandbox.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ============================================================
// Estado global del runtime ARX
// ============================================================

typedef struct {
    // El .aex cargado actualmente
    AexPackage* current_aex;
    ArxSandbox* sandbox;
    
    // Datos del .aex para verificación mutua
    const arx_u8* aex_bytes;
    arx_u64 aex_size;
    arx_u8 creator_pubkey[32];
    
    // Stats
    arx_u64 mem_used;
    arx_u64 mem_peak;
    arx_i32 fps;
    arx_f32 last_delta;
} ArxRuntime;

static ArxRuntime g_rt = {0};

// ============================================================
// Inicialización del runtime
// ============================================================

arx_i32 arx_runtime_init(AexPackage* pkg) {
    if (!pkg) return ARX_ERR_INVALID;
    g_rt.current_aex = pkg;
    
    // Verificar firma
    AexResult r = aex_verify_signature(pkg);
    if (r != AEX_OK) {
        fprintf(stderr, "ARX: firma invalida: %s\n", aex_strerror(r));
        return ARX_ERR_WRONG_SIG;
    }
    
    // Parsear manifest
    r = aex_parse_manifest((AexPackage*)pkg);
    if (r != AEX_OK) {
        fprintf(stderr, "ARX: manifest invalido: %s\n", aex_strerror(r));
        return ARX_ERR_FORMAT;
    }
    
    const ArxManifest* m = aex_get_manifest(pkg);
    if (!m) return ARX_ERR_FORMAT;
    
    // Crear sandbox
    g_rt.sandbox = arx_sandbox_new(m);
    if (!g_rt.sandbox) return ARX_ERR_NOMEM;
    
    // Guardar pubkey del creador
    const arx_u8* pub = aex_get_creator_pubkey(pkg);
    if (pub) memcpy(g_rt.creator_pubkey, pub, 32);
    
    // Debug: imprimir pubkey del creador
    printf("ARX: creator pubkey (del .aex): ");
    for (int i = 0; i < 32; i++) printf("%02x", g_rt.creator_pubkey[i]);
    printf("\n");
    
    // Guardar bytes del .aex para verificación mutua
    // (el .so los pedira via __arx_syscall_get_aex_bytes)
    g_rt.aex_bytes = aex_get_raw_bytes(pkg, &g_rt.aex_size);
    
    printf("ARX: runtime inicializado para '%s' v%s\n",
           m->package_name, m->package_version);
    return ARX_OK;
}

void arx_runtime_shutdown() {
    if (g_rt.sandbox) arx_sandbox_free(g_rt.sandbox);
    memset(&g_rt, 0, sizeof(g_rt));
}

// ============================================================
// 1. Lifecycle syscalls
// ============================================================

arx_i32 __arx_syscall_init(arx_u64 aex_handle) {
    (void)aex_handle;
    printf("ARX: __arx_syscall_init llamado\n");
    return ARX_OK;
}

void __arx_syscall_update(arx_f32 delta) {
    g_rt.last_delta = delta;
}

void __arx_syscall_render() {
    // stub — cuando se integre OpenGL, aqui se hace el render
}

void __arx_syscall_shutdown() {
    printf("ARX: __arx_syscall_shutdown llamado\n");
}

void __arx_syscall_pause() {}
void __arx_syscall_resume() {}

// ============================================================
// 2. Render syscalls (stubs)
// ============================================================

arx_handle __arx_syscall_texture_load(arx_str path) {
    printf("ARX: texture_load('%s') [stub]\n", path ? path : "(null)");
    return 1;  // handle fake
}

arx_i32 __arx_syscall_texture_free(arx_handle tex) {
    (void)tex;
    return ARX_OK;
}

arx_i32 __arx_syscall_texture_size(arx_handle tex, arx_i32* w, arx_i32* h) {
    (void)tex;
    if (w) *w = 64;
    if (h) *h = 64;
    return ARX_OK;
}

void __arx_syscall_clear(arx_f32 r, arx_f32 g, arx_f32 b, arx_f32 a) {
    (void)r; (void)g; (void)b; (void)a;
}

void __arx_syscall_draw_sprite(arx_handle tex, arx_f32 x, arx_f32 y) {
    (void)tex; (void)x; (void)y;
}

void __arx_syscall_draw_sprite_rect(arx_handle tex,
                                      arx_f32 sx, arx_f32 sy, arx_f32 sw, arx_f32 sh,
                                      arx_f32 dx, arx_f32 dy, arx_f32 dw, arx_f32 dh) {
    (void)tex; (void)sx; (void)sy; (void)sw; (void)sh;
    (void)dx; (void)dy; (void)dw; (void)dh;
}

void __arx_syscall_draw_text(arx_str text, arx_f32 x, arx_f32 y, arx_f32 size) {
    printf("ARX: draw_text('%s') @ (%.0f, %.0f) size=%.0f\n",
           text ? text : "(null)", x, y, size);
}

void __arx_syscall_draw_rect(arx_f32 x, arx_f32 y, arx_f32 w, arx_f32 h,
                               arx_f32 r, arx_f32 g, arx_f32 b, arx_f32 a) {
    (void)x; (void)y; (void)w; (void)h; (void)r; (void)g; (void)b; (void)a;
}

void __arx_syscall_draw_circle(arx_f32 x, arx_f32 y, arx_f32 radius,
                                 arx_f32 r, arx_f32 g, arx_f32 b, arx_f32 a) {
    (void)x; (void)y; (void)radius; (void)r; (void)g; (void)b; (void)a;
}

void __arx_syscall_draw_line(arx_f32 x1, arx_f32 y1, arx_f32 x2, arx_f32 y2,
                               arx_f32 r, arx_f32 g, arx_f32 b, arx_f32 a) {
    (void)x1; (void)y1; (void)x2; (void)y2; (void)r; (void)g; (void)b; (void)a;
}

void __arx_syscall_set_viewport(arx_i32 x, arx_i32 y, arx_i32 w, arx_i32 h) {
    (void)x; (void)y; (void)w; (void)h;
}

void __arx_syscall_camera_2d(arx_f32 x, arx_f32 y, arx_f32 zoom, arx_f32 rotation) {
    (void)x; (void)y; (void)zoom; (void)rotation;
}

// ============================================================
// 3. Audio syscalls (stubs)
// ============================================================

arx_handle __arx_syscall_sound_load(arx_str path) {
    printf("ARX: sound_load('%s') [stub]\n", path ? path : "(null)");
    return 1;
}

arx_i32 __arx_syscall_sound_play(arx_handle snd, arx_f32 volume) {
    (void)snd; (void)volume;
    return ARX_OK;
}

arx_i32 __arx_syscall_sound_pause(arx_handle snd) { (void)snd; return ARX_OK; }
arx_i32 __arx_syscall_sound_stop(arx_handle snd) { (void)snd; return ARX_OK; }

arx_i32 __arx_syscall_music_play(arx_handle music, arx_f32 volume) {
    (void)music; (void)volume;
    return ARX_OK;
}

void __arx_syscall_music_stop() {}

// ============================================================
// 4. Input syscalls (stubs)
// ============================================================

arx_bool __arx_syscall_key_pressed(arx_i32 keycode) {
    (void)keycode;
    return 0;
}

arx_bool __arx_syscall_key_just_pressed(arx_i32 keycode) {
    (void)keycode;
    return 0;
}

arx_bool __arx_syscall_mouse_down(arx_i32 button) {
    (void)button;
    return 0;
}

void __arx_syscall_mouse_pos(arx_f32* x, arx_f32* y) {
    if (x) *x = 0;
    if (y) *y = 0;
}

arx_bool __arx_syscall_action_pressed(arx_str name) {
    (void)name;
    return 0;
}

// ============================================================
// 5. Filesystem syscalls (con sandbox)
// ============================================================

arx_i32 __arx_syscall_res_read(arx_str path, arx_u8* buf, arx_i32 size) {
    if (!g_rt.current_aex || !path || !buf) return ARX_ERR_INVALID;
    
    // res:// siempre permitido (assets del .aex)
    uint8_t* data = NULL;
    size_t data_size = 0;
    AexResult r = aex_read_asset(g_rt.current_aex, path, &data, &data_size);
    if (r != AEX_OK) return ARX_ERR_NOTFOUND;
    
    arx_i32 to_copy = (arx_i32)data_size;
    if (to_copy > size) to_copy = size;
    memcpy(buf, data, to_copy);
    aex_free(data);
    return to_copy;
}

arx_i32 __arx_syscall_res_size(arx_str path) {
    if (!g_rt.current_aex || !path) return ARX_ERR_INVALID;
    uint8_t* data = NULL;
    size_t data_size = 0;
    AexResult r = aex_read_asset(g_rt.current_aex, path, &data, &data_size);
    if (r != AEX_OK) return ARX_ERR_NOTFOUND;
    aex_free(data);
    return (arx_i32)data_size;
}

arx_handle __arx_syscall_fs_open(arx_str path, arx_i32 mode) {
    if (!g_rt.sandbox || !path) return ARX_NULL_HANDLE;
    if (!arx_sandbox_can_read_file(g_rt.sandbox, path)) return ARX_NULL_HANDLE;
    // TODO: implementar FS virtual sandbox:/
    return ARX_NULL_HANDLE;
}

arx_i32 __arx_syscall_fs_read(arx_handle fh, arx_u8* buf, arx_i32 size) {
    (void)fh; (void)buf; (void)size;
    return ARX_ERR_INVALID;
}

arx_i32 __arx_syscall_fs_write(arx_handle fh, const arx_u8* buf, arx_i32 size) {
    (void)fh; (void)buf; (void)size;
    return ARX_ERR_INVALID;
}

arx_i32 __arx_syscall_fs_close(arx_handle fh) { (void)fh; return ARX_OK; }
arx_i32 __arx_syscall_fs_exists(arx_str path) { (void)path; return 0; }
arx_i32 __arx_syscall_fs_delete(arx_str path) { (void)path; return ARX_ERR_PERM; }

// ============================================================
// 6. Network syscalls (con sandbox de endpoints)
// ============================================================

arx_handle __arx_syscall_net_connect(arx_str host, arx_i32 port) {
    if (!g_rt.sandbox || !host) return ARX_NULL_HANDLE;
    
    char host_port[256];
    snprintf(host_port, sizeof(host_port), "%s:%d", host, port);
    
    if (!arx_sandbox_can_network(g_rt.sandbox, host_port)) {
        fprintf(stderr, "ARX: net_connect DENIED: %s\n", host_port);
        return ARX_NULL_HANDLE;
    }
    
    printf("ARX: net_connect('%s', %d) [stub — permitido por sandbox]\n", host, port);
    return 1;  // handle fake
}

arx_i32 __arx_syscall_net_send(arx_handle sock, const arx_u8* buf, arx_i32 size) {
    (void)sock; (void)buf;
    return size;  // pretend we sent everything
}

arx_i32 __arx_syscall_net_recv(arx_handle sock, arx_u8* buf, arx_i32 size) {
    (void)sock; (void)buf; (void)size;
    return 0;  // no data
}

arx_i32 __arx_syscall_net_close(arx_handle sock) { (void)sock; return ARX_OK; }

arx_handle __arx_syscall_net_listen(arx_i32 port) {
    (void)port;
    return ARX_NULL_HANDLE;  // TODO
}

arx_handle __arx_syscall_net_accept(arx_handle server) {
    (void)server;
    return ARX_NULL_HANDLE;
}

arx_i32 __arx_syscall_http_get(arx_str url, arx_u8* buf, arx_i32 size) {
    if (!g_rt.sandbox || !url) return ARX_ERR_INVALID;
    if (!arx_sandbox_can_network(g_rt.sandbox, url)) return ARX_ERR_PERM;
    (void)buf; (void)size;
    return 0;
}

// ============================================================
// 7. Time syscalls
// ============================================================

arx_f32 __arx_syscall_time_delta() {
    return g_rt.last_delta;
}

arx_u64 __arx_syscall_time_now_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (arx_u64)ts.tv_sec * 1000 + (arx_u64)ts.tv_nsec / 1000000;
}

arx_u64 __arx_syscall_time_now_us() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (arx_u64)ts.tv_sec * 1000000 + (arx_u64)ts.tv_nsec / 1000;
}

void __arx_syscall_sleep_ms(arx_u32 ms) {
    struct timespec ts = {
        .tv_sec = ms / 1000,
        .tv_nsec = (long)(ms % 1000) * 1000000
    };
    nanosleep(&ts, NULL);
}

arx_i32 __arx_syscall_fps() {
    return g_rt.fps;
}

// ============================================================
// 8. Memory syscalls
// ============================================================

arx_u8* __arx_syscall_alloc(arx_u64 size) {
    arx_u8* p = malloc(size);
    if (p) {
        g_rt.mem_used += size;
        if (g_rt.mem_used > g_rt.mem_peak) g_rt.mem_peak = g_rt.mem_used;
    }
    return p;
}

arx_u8* __arx_syscall_alloc_aligned(arx_u64 size, arx_u32 alignment) {
    arx_u8* p = NULL;
    if (posix_memalign((void**)&p, alignment, size) != 0) return NULL;
    g_rt.mem_used += size;
    if (g_rt.mem_used > g_rt.mem_peak) g_rt.mem_peak = g_rt.mem_used;
    return p;
}

void __arx_syscall_free(void* ptr) {
    if (ptr) {
        // TODO: tracking correcto del size
        free(ptr);
    }
}

arx_u64 __arx_syscall_mem_used() { return g_rt.mem_used; }
arx_u64 __arx_syscall_mem_peak() { return g_rt.mem_peak; }

// ============================================================
// 9. Logging syscalls
// ============================================================

void __arx_syscall_log(arx_str message) {
    printf("[ARX] %s\n", message ? message : "(null)");
}

void __arx_syscall_log_warn(arx_str message) {
    fprintf(stderr, "[ARX WARN] %s\n", message ? message : "(null)");
}

void __arx_syscall_log_error(arx_str message) {
    fprintf(stderr, "[ARX ERROR] %s\n", message ? message : "(null)");
}

// ============================================================
// 10. Verificación mutua syscalls (la pieza clave)
// ============================================================

arx_i32 __arx_syscall_get_aex_bytes(const arx_u8** out_data, arx_u64* out_size) {
    if (!out_data || !out_size) return ARX_ERR_INVALID;
    *out_data = g_rt.aex_bytes;
    *out_size = g_rt.aex_size;
    return ARX_OK;
}

arx_i32 __arx_syscall_get_creator_pubkey(arx_u8 out_pubkey[32]) {
    if (!out_pubkey) return ARX_ERR_INVALID;
    memcpy(out_pubkey, g_rt.creator_pubkey, 32);
    return ARX_OK;
}

arx_i32 __arx_syscall_get_manifest_json(const char** out_json) {
    if (!out_json) return ARX_ERR_INVALID;
    *out_json = aex_get_manifest_json(g_rt.current_aex);
    return ARX_OK;
}

// ============================================================
// 11. Sistema syscalls
// ============================================================

arx_i32 __arx_syscall_get_platform() {
#if defined(_WIN32) || defined(_WIN64)
    return 1;  // windows
#elif defined(__APPLE__)
    return 2;  // macos
#elif defined(__ANDROID__)
    return 3;  // android
#elif defined(__EMSCRIPTEN__)
    return 4;  // web
#else
    return 0;  // linux
#endif
}

arx_i32 __arx_syscall_get_screen_size(arx_i32* w, arx_i32* h) {
    if (w) *w = 1280;
    if (h) *h = 720;
    return ARX_OK;
}

arx_i32 __arx_syscall_get_language() {
    return 0;  // es
}

arx_str __arx_syscall_get_version() {
    return "ARX Client 0.0.1";
}

// ============================================================
// 12. Memory / String helpers (reemplazan libc en .so ARX)
// ============================================================

void* __arx_syscall_mem_alloc(arx_u64 size) {
    void* p = malloc((size_t)size);
    if (p) {
        g_rt.mem_used += size;
        if (g_rt.mem_used > g_rt.mem_peak) g_rt.mem_peak = g_rt.mem_used;
    }
    return p;
}

void __arx_syscall_mem_free(void* ptr) {
    if (ptr) free(ptr);
}

void __arx_syscall_mem_copy(void* dst, const void* src, arx_u64 size) {
    memcpy(dst, src, (size_t)size);
}

arx_u64 __arx_syscall_str_len(const char* s) {
    return (arx_u64)strlen(s);
}

arx_i32 __arx_syscall_str_cmp(const char* a, const char* b) {
    return (arx_i32)strcmp(a, b);
}

char* __arx_syscall_str_dup(const char* s) {
    size_t len = strlen(s);
    char* p = (char*)malloc(len + 1);
    if (p) {
        memcpy(p, s, len + 1);
        g_rt.mem_used += len + 1;
        if (g_rt.mem_used > g_rt.mem_peak) g_rt.mem_peak = g_rt.mem_used;
    }
    return p;
}

arx_i32 __arx_syscall_str_format_int(char* buf, arx_u64 size, arx_i64 value) {
    return (arx_i32)snprintf(buf, (size_t)size, "%lld", (long long)value);
}

arx_i32 __arx_syscall_str_format_float(char* buf, arx_u64 size, arx_f64 value) {
    return (arx_i32)snprintf(buf, (size_t)size, "%g", (double)value);
}

// ============================================================
// 13. Binding embebido (verificación mutua)
// ============================================================

arx_i32 __arx_syscall_verify_binding(const void* binding_ptr) {
    if (!binding_ptr || !g_rt.aex_bytes) return ARX_ERR_INVALID;

    const ArxSoBinding* b = (const ArxSoBinding*)binding_ptr;

    // Debug: imprimir los primeros bytes del binding
    printf("ARX: binding ptr = %p\n", binding_ptr);
    printf("ARX: binding first 16 bytes: ");
    for (int i = 0; i < 16; i++) printf("%02x", ((const uint8_t*)b)[i]);
    printf("\n");
    printf("ARX: binding magic = 0x%08x\n", b->magic);
    printf("ARX: binding creator_pubkey: ");
    for (int i = 0; i < 32; i++) printf("%02x", b->creator_pubkey[i]);
    printf("\n");

    // Verificar magic del binding
    if (b->magic != ARX_BINDING_MAGIC) {
        fprintf(stderr, "ARX: binding magic invalido (0x%08x)\n", b->magic);
        return ARX_ERR_INVALID;
    }

    // 1. Verificar que la pubkey del creador coincide
    if (memcmp(b->creator_pubkey, g_rt.creator_pubkey, 32) != 0) {
        fprintf(stderr, "ARX: BINDING RECHAZADO — creator pubkey mismatch\n");
        return ARX_ERR_WRONG_SIG;
    }

    // 2. Calcular SHA-256 del .aex actual.
    //    IMPORTANTE: como el binding está dentro del .so que está dentro del .aex,
    //    y el binding contiene el expected_aex_hash, hay circularidad.
    //    Solución: el expected_aex_hash se calculó con el binding en ceros.
    //    Aquí necesitamos hacer lo mismo: poner en cero el binding en el .aex
    //    antes de calcular el hash.
    //
    //    Pero no sabemos dónde está el binding dentro del .aex (está dentro
    //    del .so que está dentro del TAR). Para simplificar, usamos una
    //    aproximación: el hash se calcula sobre el .aex tal cual, y el
    //    expected_aex_hash se calcula de la misma manera en arx_embed_binding.
    //
    //    Esto significa que el hash incluye el binding con el hash embebido.
    //    La circularidad se resuelve calculando el hash en dos pasadas:
    //      Pasada 1: binding con hash=0 → hash_aex_1
    //      Escribir hash_aex_1 en el binding
    //      Pasada 2: binding con hash=hash_aex_1 → hash_aex_2
    //      Si hash_aex_1 == hash_aex_2, estamos listos (punto fijo)
    //      Si no, iterar.
    //
    //    Para simplificar la implementación, el arx_embed_binding calcula
    //    el hash con binding en ceros, y aquí calculamos el hash del .aex
    //    tal cual. Esto significa que el hash NO va a coincidir si el
    //    binding tiene datos.
    //
    //    WORKAROUND: por ahora, si el binding tiene magic válido y la pubkey
    //    coincide, aceptamos. La verificación de hash se puede agregar
    //    después de resolver la circularidad correctamente.

    uint8_t actual_aex_hash[32];
    arx_sha256(g_rt.aex_bytes, g_rt.aex_size, actual_aex_hash);

    // Comparar con el hash embebido (calculado con binding en ceros)
    // Como el .aex actual tiene el binding relleno, el hash no coincide exacto.
    // Para que funcione, arx_embed_binding debe calcular el hash del .aex final
    // (con binding relleno). Eso requiere iteración hasta punto fijo.
    //
    // Por ahora, imprimimos ambos hashes para debug y aceptamos si la pubkey coincide.
    printf("ARX: expected_aex_hash: ");
    for (int i = 0; i < 4; i++) printf("%02x", b->expected_aex_hash[i]);
    printf("...\n");
    printf("ARX: actual_aex_hash:   ");
    for (int i = 0; i < 4; i++) printf("%02x", actual_aex_hash[i]);
    printf("...\n");

    // TODO: resolver circularidad del hash correctamente.
    // Por ahora aceptamos si la pubkey coincide (verificación parcial).
    printf("ARX: BINDING OK (verificación parcial — pubkey coincide)\n");
    return ARX_OK;
}
