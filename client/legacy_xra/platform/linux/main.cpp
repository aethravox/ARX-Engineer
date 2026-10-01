// =============================================================
// ARX ENGINE - CLIENTE (Visor)
// Compilar con: scons  (usando SConstruct)
// Uso: ./ARX_Cliente [archivo.asx]
// Librerias: raylib, assimp, imgui, Jolt, glm, zlib
// =============================================================
#include "raylib.h"
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include "../../../Motor/platform/linux/core/AethravoxCommon.hpp"
#include "../../../Motor/platform/linux/resources/AethravoxResources.hpp"
#include "../../../Motor/platform/linux/graphics/AethravoxGraphics.hpp"
#include "scripting/AethravoxAsxLoader.hpp"
#include "ARX_Math.hpp"

int main(int argc, char* argv[]) {
    const int vW = 800, vH = 600;
    std::string rutaAsx = (argc > 1) ? argv[1] : "main.asx";

    // Inicializar ventana
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(vW, vH, "ARX Engine - Cliente (v2.0)");
    SetTargetFPS(60);

    // Inicializar audio
    RenderEngine::InitAudio();

    // Inicializar fisica (para escenas con rigidbody)
    RenderEngine::InitPhysics(true);

    // Cargar paquete .asx comprimido
    if (!AethravoxAsxLoader::CargarPaqueteCompleto(rutaAsx)) {
        std::cout << "[Error] No se pudo cargar el paquete: " << rutaAsx << std::endl;

        while (!WindowShouldClose()) {
            BeginDrawing();
                ClearBackground({30, 30, 50, 255});
                DrawText("ERROR: No se pudo cargar el archivo .asx", 150, 250, 20, RED);
                DrawText(rutaAsx.c_str(), 200, 290, 16, WHITE);
                DrawText("Asegurate de que el archivo existe y no esta corrupto.", 120, 330, 14, GRAY);
            EndDrawing();
        }
        CloseWindow();
        return -1;
    }

    // Cargar paleta desde memoria (con fallback a default)
    AssetManager assets;
    std::unordered_map<std::string, Color> paleta;
    if (AethravoxAsxLoader::MemoriaVirtual.count("color.aex")) {
        paleta = assets.CargarPaletaDesdeMemoria(AethravoxAsxLoader::MemoriaVirtual["color.aex"]);
    } else {
        paleta = Aethravox::PaletaDefault();
        std::cout << "[Cliente] color.aex no encontrado en paquete, usando paleta default" << std::endl;
    }
    Aethravox::paletaColores = paleta;

    // Cargar escena inicial desde memoria
    std::vector<Elemento> escena;
    if (AethravoxAsxLoader::MemoriaVirtual.count("config.aex")) {
        escena = assets.CargarEscenaDesdeMemoria(AethravoxAsxLoader::MemoriaVirtual["config.aex"]);
    } else {
        std::cout << "[Cliente] config.aex no encontrado en paquete, escena vacia" << std::endl;
    }

    // Registrar comandos basicos del cliente
    CommandDispatcher::Get().RegisterCommand("ir a", [&](const std::string& params) {
        std::string archivoDestino = params + ".aex";
        if (AethravoxAsxLoader::MemoriaVirtual.count(archivoDestino)) {
            escena = assets.CargarEscenaDesdeMemoria(AethravoxAsxLoader::MemoriaVirtual[archivoDestino]);
            Aethravox::escenaActual = archivoDestino;
            std::cout << "[Cliente] Cambiando a: " << archivoDestino << std::endl;
        } else {
            std::cout << "[Cliente] Escena no encontrada en paquete: " << archivoDestino << std::endl;
        }
    });

    CommandDispatcher::Get().RegisterCommand("salir", [](const std::string&) {
        RenderEngine::Cleanup();
        CloseWindow();
    });

    RenderTexture2D target = LoadRenderTexture(vW, vH);

    std::cout << "\n========================================" << std::endl;
    std::cout << "  ARX Engine v2.0 - Cliente Iniciado" << std::endl;
    std::cout << "  Paquete: " << rutaAsx << std::endl;
    std::cout << "  Archivos: " << AethravoxAsxLoader::MemoriaVirtual.size() << std::endl;
    std::cout << "  Elementos: " << escena.size() << std::endl;
    std::cout << "========================================\n" << std::endl;

    // ===== BUCLE PRINCIPAL =====
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // Coordenadas virtuales
        float sX = (float)GetScreenWidth() / (float)vW;
        float sY = (float)GetScreenHeight() / (float)vH;
        Vector2 mRaw = GetMousePosition();
        Vector2 vMouse = { mRaw.x / sX, mRaw.y / sY };

        // Click en botones
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (!RenderEngine::imguiEnabled || !ARX::ARXImGui::IsMouseOverImGui()) {
                for (auto& el : escena) {
                    if (el.tipo == "boton" && el.props.count("accion")) {
                        if (CheckCollisionPointRec(vMouse, el.rect)) {
                            std::string accion = el.props.at("accion");
                            size_t spacePos = accion.find(' ');
                            if (spacePos != std::string::npos) {
                                std::string cmd = accion.substr(0, spacePos);
                                std::string params = accion.substr(spacePos + 1);
                                CommandDispatcher::Get().Execute(cmd, params);
                            } else {
                                CommandDispatcher::Get().Execute(accion, "");
                            }
                        }
                    }
                }
            }
        }

        // Fisica
        RenderEngine::StepPhysics(dt);

        // Render
        BeginTextureMode(target);
            ClearBackground(BLACK);

            // Separar elementos 2D y 3D
            bool tiene3D = std::any_of(escena.begin(), escena.end(),
                [](const Elemento& el) { return RenderEngine::IsType3D(el.tipo); });

            // Dibujar 3D primero (si hay elementos 3D)
            if (tiene3D) {
                RenderEngine::InitCamera3D();
                BeginMode3D(RenderEngine::camera3D);
                for (const auto& el : escena) {
                    if (RenderEngine::IsType3D(el.tipo)) {
                        RenderEngine::DrawElement(el, paleta);
                    }
                }
                EndMode3D();
            }

            // Dibujar solo 2D
            for (const auto& el : escena) {
                if (!RenderEngine::IsType3D(el.tipo)) {
                    RenderEngine::DrawElement(el, paleta);
                }
            }

        EndTextureMode();

        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexturePro(
                target.texture,
                {0, 0, (float)vW, -(float)vH},
                {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
                {0, 0}, 0.0f, WHITE
            );
            DrawFPS(10, 10);
        EndDrawing();

        // Actualizar audio
        RenderEngine::UpdateMusicStreams();
    }

    RenderEngine::Cleanup();
    UnloadRenderTexture(target);
    CloseWindow();

    std::cout << "\n=== ARX Engine - Cliente Cerrado ===" << std::endl;
    return 0;
}
