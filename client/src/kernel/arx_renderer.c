// arx_renderer.c — Implementación OpenGL de las syscalls de render de ARX OS.
//
// Usa GLFW para crear la ventana y OpenGL (legacy/immediate mode para simplicidad).
// Cuando el .so llama a __arx_syscall_clear / draw_rect / draw_text, estas
// funciones dibujan en la ventana.

#include "arx/os.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static GLFWwindow* g_window = NULL;
static int g_screen_w = 1280;
static int g_screen_h = 720;

// Inicializar el renderer (abrir ventana)
int arx_renderer_init(int width, int height) {
    if (g_window) return 0;  // ya inicializado

    g_screen_w = width;
    g_screen_h = height;

    if (!glfwInit()) {
        fprintf(stderr, "ARX: FAIL glfwInit\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    g_window = glfwCreateWindow(width, height, "ARX Client", NULL, NULL);
    if (!g_window) {
        fprintf(stderr, "ARX: FAIL glfwCreateWindow\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(g_window);
    glfwSwapInterval(1);  // vsync

    // Configurar OpenGL ortho 2D (origen arriba-izquierda como canvas)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);  // y invertido (0=arriba)
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Color de fondo por defecto (dark)
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    printf("ARX: ventana creada (%dx%d)\n", width, height);
    return 0;
}

void arx_renderer_shutdown() {
    if (g_window) {
        glfwDestroyWindow(g_window);
        g_window = NULL;
    }
    glfwTerminate();
}

// Si la ventana debe cerrarse (usuario presionó X o ESC)
int arx_renderer_should_close() {
    return g_window ? (glfwWindowShouldClose(g_window) || glfwGetKey(g_window, GLFW_KEY_ESCAPE)) : 1;
}

// Swap buffers + poll events
void arx_renderer_swap() {
    if (g_window) {
        glfwSwapBuffers(g_window);
        glfwPollEvents();
    }
}

// ============================================================
// Implementación de syscalls de render
// ============================================================

void __arx_syscall_clear(arx_f32 r, arx_f32 g, arx_f32 b, arx_f32 a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void __arx_syscall_draw_rect(arx_f32 x, arx_f32 y, arx_f32 w, arx_f32 h,
                               arx_f32 r, arx_f32 g, arx_f32 b, arx_f32 a) {
    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void __arx_syscall_draw_circle(arx_f32 cx, arx_f32 cy, arx_f32 radius,
                                 arx_f32 r, arx_f32 g, arx_f32 b, arx_f32 a) {
    glColor4f(r, g, b, a);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 32; i++) {
        float angle = (float)i / 32.0f * 6.2831853f;
        glVertex2f(cx + radius * __builtin_cosf(angle),
                   cy + radius * __builtin_sinf(angle));
    }
    glEnd();
}

void __arx_syscall_draw_line(arx_f32 x1, arx_f32 y1, arx_f32 x2, arx_f32 y2,
                               arx_f32 r, arx_f32 g, arx_f32 b, arx_f32 a) {
    glColor4f(r, g, b, a);
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

void __arx_syscall_draw_text(arx_str text, arx_f32 x, arx_f32 y, arx_f32 size) {
    // Por ahora, dibujar texto como rectángulos de píxeles (bitmap font simple)
    // En el futuro, usar FreeType o stb_truetype.
    // Por ahora, loguear + dibujar un rectángulo de fondo.
    if (!text) return;

    // Dibujar un fondo oscuro detrás del texto
    float text_w = (float)strlen(text) * size * 0.6f;
    glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(x - 2, y - 2);
    glVertex2f(x + text_w + 2, y - 2);
    glVertex2f(x + text_w + 2, y + size + 2);
    glVertex2f(x - 2, y + size + 2);
    glEnd();

    // Dibujar cada caracter como puntos simples (placeholder)
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glBegin(GL_POINTS);
    for (size_t i = 0; i < strlen(text); i++) {
        float cx = x + (float)i * size * 0.6f + size * 0.3f;
        float cy = y + size * 0.5f;
        // Cada caracter es un patrón de puntos diferente (basado en el ASCII)
        unsigned char c = text[i];
        for (int py = 0; py < 5; py++) {
            for (int px = 0; px < 3; px++) {
                // Patrón pseudo-aleatorio pero determinista por caracter
                if ((c >> (px + py)) & 1) {
                    glVertex2f(cx + px * size * 0.1f, cy + py * size * 0.1f - size * 0.25f);
                }
            }
        }
    }
    glEnd();
}

// Stubs para las que no necesitamos aun
arx_handle __arx_syscall_texture_load(arx_str path) {
    (void)path; return ARX_NULL_HANDLE;
}
arx_i32 __arx_syscall_texture_free(arx_handle tex) { (void)tex; return ARX_OK; }
arx_i32 __arx_syscall_texture_size(arx_handle tex, arx_i32* w, arx_i32* h) {
    (void)tex; if (w) *w = 64; if (h) *h = 64; return ARX_OK;
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
void __arx_syscall_set_viewport(arx_i32 x, arx_i32 y, arx_i32 w, arx_i32 h) {
    (void)x; (void)y; (void)w; (void)h;
}
void __arx_syscall_camera_2d(arx_f32 x, arx_f32 y, arx_f32 zoom, arx_f32 rotation) {
    (void)x; (void)y; (void)zoom; (void)rotation;
}

// Input
arx_bool __arx_syscall_key_pressed(arx_i32 keycode) {
    if (!g_window) return 0;
    // Mapear keycodes ARX a GLFW
    int glfw_key = 0;
    if (keycode >= 1 && keycode <= 26) {  // A-Z
        glfw_key = GLFW_KEY_A + (keycode - 1);
    } else if (keycode == 37) {
        glfw_key = GLFW_KEY_SPACE;
    } else if (keycode == 38) {
        glfw_key = GLFW_KEY_ENTER;
    } else if (keycode == 39) {
        glfw_key = GLFW_KEY_ESCAPE;
    } else if (keycode == 100) {
        glfw_key = GLFW_KEY_UP;
    } else if (keycode == 101) {
        glfw_key = GLFW_KEY_DOWN;
    } else if (keycode == 102) {
        glfw_key = GLFW_KEY_LEFT;
    } else if (keycode == 103) {
        glfw_key = GLFW_KEY_RIGHT;
    }
    return glfwGetKey(g_window, glfw_key) == GLFW_PRESS;
}

arx_bool __arx_syscall_key_just_pressed(arx_i32 keycode) {
    return __arx_syscall_key_pressed(keycode);  // simplificado
}

arx_bool __arx_syscall_mouse_down(arx_i32 button) {
    if (!g_window) return 0;
    return glfwGetMouseButton(g_window, button) == GLFW_PRESS;
}

void __arx_syscall_mouse_pos(arx_f32* x, arx_f32* y) {
    if (!g_window || !x || !y) { if (x) *x = 0; if (y) *y = 0; return; }
    double mx, my;
    glfwGetCursorPos(g_window, &mx, &my);
    *x = (arx_f32)mx;
    *y = (arx_f32)my;
}

arx_bool __arx_syscall_action_pressed(arx_str name) { (void)name; return 0; }

// Screen size
arx_i32 __arx_syscall_get_screen_size(arx_i32* w, arx_i32* h) {
    if (w) *w = g_screen_w;
    if (h) *h = g_screen_h;
    return ARX_OK;
}
