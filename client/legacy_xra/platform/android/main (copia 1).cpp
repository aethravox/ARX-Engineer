// =============================================================
// ARX ENGINE - CLIENTE (Visor) - ANDROID
// Version adaptada para touch inputs
// Compilar con Android NDK + CMake
// Librerias: raylib, assimp, imgui, Jolt, glm, zlib
// =============================================================
#include "raylib.h"
#include <android_native_app_glue.h>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include "../../../Motor/platform/android/core/AethravoxCommon.hpp"
#include "../../../Motor/platform/android/resources/AethravoxResources.hpp"
#include "../../../Motor/platform/android/graphics/AethravoxGraphics.hpp"
#include "scripting/AethravoxAsxLoader.hpp"
#include "ARX_Math.hpp"

Vector2 GetVirtualTouch(int vW, int vH) {
    float sX = (float)GetScreenWidth() / (float)vW;
    float sY = (float)GetScreenHeight() / (float)vH;
    Vector2 t;
#if defined(PLATFORM_ANDROID)
    t.x = (float)GetTouchX() / sX;
    t.y = (float)GetTouchY() / sY;
#else
    t = GetMousePosition();
    t.x /= sX;
    t.y /= sY;
#endif
    return t;
}

bool IsTouchPressed() {
#if defined(PLATFORM_ANDROID)
    return IsGestureDetected(GESTURE_TAP);
#else
    return IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
#endif
}

int main(int argc, char* argv[]) {
    const int vW = 800, vH = 600;
    std::string rutaAsx = (argc > 1) ? argv[1] : "main.asx";

    InitWindow(vW, vH, "ARX Engine - Cliente");
    SetTargetFPS(60);
    RenderEngine::InitAudio();
    RenderEngine::InitPhysics(true);

    if (!AethravoxAsxLoader::CargarPaqueteCompleto(rutaAsx)) {
        while (!WindowShouldClose()) {
            BeginDrawing();
                ClearBackground({30, 30, 50, 255});
                DrawText("Error cargando .asx", 150, 280, 20, RED);
            EndDrawing();
        }
        CloseWindow();
        return -1;
    }

    AssetManager assets;
    std::unordered_map<std::string, Color> paleta;
    if (AethravoxAsxLoader::MemoriaVirtual.count("color.aex"))
        paleta = assets.CargarPaletaDesdeMemoria(AethravoxAsxLoader::MemoriaVirtual["color.aex"]);
    else
        paleta = Aethravox::PaletaDefault();
    Aethravox::paletaColores = paleta;

    std::vector<Elemento> escena;
    if (AethravoxAsxLoader::MemoriaVirtual.count("config.aex"))
        escena = assets.CargarEscenaDesdeMemoria(AethravoxAsxLoader::MemoriaVirtual["config.aex"]);

    CommandDispatcher::Get().RegisterCommand("ir a", [&](const std::string& params) {
        std::string dest = params + ".aex";
        if (AethravoxAsxLoader::MemoriaVirtual.count(dest))
            escena = assets.CargarEscenaDesdeMemoria(AethravoxAsxLoader::MemoriaVirtual[dest]);
    });
    CommandDispatcher::Get().RegisterCommand("salir", [](const std::string&) {
        RenderEngine::Cleanup(); CloseWindow();
    });

    RenderTexture2D target = LoadRenderTexture(vW, vH);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        Vector2 vTouch = GetVirtualTouch(vW, vH);

        RenderEngine::StepPhysics(dt);

        if (IsTouchPressed()) {
            for (auto& el : escena) {
                if (el.tipo == "boton" && el.props.count("accion")) {
                    if (CheckCollisionPointRec(vTouch, el.rect)) {
                        std::string accion = el.props.at("accion");
                        size_t sp = accion.find(' ');
                        if (sp != std::string::npos)
                            CommandDispatcher::Get().Execute(accion.substr(0, sp), accion.substr(sp + 1));
                        else
                            CommandDispatcher::Get().Execute(accion, "");
                    }
                }
            }
        }

        BeginTextureMode(target);
            ClearBackground(BLACK);
            bool tiene3D = std::any_of(escena.begin(), escena.end(),
                [](const Elemento& el) { return RenderEngine::IsType3D(el.tipo); });
            if (tiene3D) {
                RenderEngine::InitCamera3D();
                BeginMode3D(RenderEngine::camera3D);
                for (const auto& el : escena) {
                    if (RenderEngine::IsType3D(el.tipo))
                        RenderEngine::DrawElement(el, paleta);
                }
                EndMode3D();
            }
            for (const auto& el : escena) {
                if (!RenderEngine::IsType3D(el.tipo))
                    RenderEngine::DrawElement(el, paleta);
            }
        EndTextureMode();

        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexturePro(target.texture,
                {0, 0, (float)vW, -(float)vH},
                {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
                {0, 0}, 0.0f, WHITE);
            DrawFPS(10, 10);
        EndDrawing();

        RenderEngine::UpdateMusicStreams();
    }

    RenderEngine::Cleanup();
    UnloadRenderTexture(target);
    CloseWindow();
    return 0;
}
