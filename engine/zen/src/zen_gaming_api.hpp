// ==============================================================================
// zen/src/zen_gaming_api.hpp — API de gaming para Zen (input, audio, etc.)
//
// Este header se incluye en codegen.cpp y declara las funciones extern
// que el motor provee para que Zen las llame.
// Las funciones reales están implementadas en el motor (input.cpp, audio_server.cpp, etc.)
// y se linkean cuando el programa final se linkea contra libarx_core.a
// ==============================================================================
#pragma once

// === INPUT ===
// Key codes (compatibles con GLFW)
// W=87, A=65, S=83, D=68, ESC=256, SPACE=32, ENTER=257, etc.

// Estas funciones las provee el runtime del motor (arx_runtime)
// cuando el programa se ejecuta dentro del motor.
// Cuando se compila standalone, el programa debe linkear contra libarx_core.a

#ifdef __cplusplus
extern "C" {
#endif

// Input - teclado
int arx_key_down(int key);        // true mientras la tecla está presionada
int arx_key_pressed(int key);     // true solo el frame que se presionó
int arx_key_released(int key);    // true solo el frame que se soltó

// Input - mouse
int arx_mouse_down(int button);   // 0=left, 1=right, 2=middle
int arx_mouse_pressed(int button);
double arx_mouse_x(void);         // posición X del mouse
double arx_mouse_y(void);         // posición Y del mouse
double arx_mouse_wheel(void);     // scroll wheel

// Input - gamepad
int arx_gamepad_connected(int pad);
double arx_gamepad_axis(int pad, int axis);  // -1.0 a 1.0
int arx_gamepad_button(int pad, int button);

// Audio
void arx_audio_play(const char* path);
void arx_audio_stop(void);
void arx_audio_set_volume(double vol);
double arx_audio_get_volume(void);

// Physics
int arx_physics_raycast(double x, double y, double z,
                          double dx, double dy, double dz,
                          double max_dist);
void arx_physics_set_gravity(double x, double y, double z);

// Render 2D
void arx_draw_rect(double x, double y, double w, double h,
                     double r, double g, double b, double a);
void arx_draw_circle(double x, double y, double radius,
                       double r, double g, double b, double a);
void arx_draw_text(const char* text, double x, double y,
                     double r, double g, double b);
void arx_draw_line(double x1, double y1, double x2, double y2,
                     double r, double g, double b);

// Engine info
double arx_get_fps(void);
double arx_get_delta(void);
void arx_quit(void);

#ifdef __cplusplus
}
#endif
