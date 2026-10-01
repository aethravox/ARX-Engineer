// =============================================================
// ARX ENGINE - Modulo ImGui (UI Inmediata)
// Integracion de Dear ImGui con Raylib como backend
// Compatible con ImGui 1.87+ (API moderna con AddKeyEvent)
// =============================================================
#ifndef ARX_IMGUI_HPP
#define ARX_IMGUI_HPP

#include "imgui.h"
#include "raylib.h"
#include "rlgl.h"
#include <string>
#include <vector>
#include <functional>
#include <iostream>

namespace ARX {

class ARXImGui {
public:
    static inline bool initialized = false;
    static inline ImGuiContext* context = nullptr;
    static inline Texture2D fontTexture = { 0 };

    static void Init() {
        if (initialized) return;

        context = ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
#ifdef ImGuiConfigFlags_DockingEnable
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Habilitar docking si esta disponible
#endif
        ImGui::StyleColorsDark();

        // 1. Crear Textura de Fuente
        unsigned char* pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

        // Raylib pixel formats (tu build falló por UNCOMPRESSED_R8G8B8A8 no definido)
        #ifndef PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
        #define PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 14
        #endif

        Image image = { pixels, width, height, 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 };
        fontTexture = LoadTextureFromImage(image);

        // ImGui usa ImTextureID (normalmente 64-bit). Para raylib, convertimos GLuint -> ImTextureID.
        io.Fonts->TexID = (ImTextureID)(intptr_t)fontTexture.id;

        initialized = true;
        std::cout << "[ARXImGui] Inicializado con textura de fuente ID: " << fontTexture.id << std::endl;
    }

    static void NewFrame(int screenWidth, int screenHeight) {
        if (!initialized) Init();

        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2((float)screenWidth, (float)screenHeight);
        io.DeltaTime = GetFrameTime();

        // Mouse & Wheel
        Vector2 mousePos = GetMousePosition();
        io.AddMousePosEvent(mousePos.x, mousePos.y);
        io.AddMouseButtonEvent(0, IsMouseButtonDown(MOUSE_LEFT_BUTTON));
        io.AddMouseButtonEvent(1, IsMouseButtonDown(MOUSE_RIGHT_BUTTON));
        io.AddMouseButtonEvent(2, IsMouseButtonDown(MOUSE_MIDDLE_BUTTON));
        io.AddMouseWheelEvent(0.0f, GetMouseWheelMove());

        // Keyboard mapping (ImGui 1.87+)
        #define ARX_MAP_KEY(ImKey, RayKey) if (IsKeyPressed(RayKey)) io.AddKeyEvent(ImKey, true); if (IsKeyReleased(RayKey)) io.AddKeyEvent(ImKey, false);
        ARX_MAP_KEY(ImGuiKey_Tab, KEY_TAB);
        ARX_MAP_KEY(ImGuiKey_LeftArrow, KEY_LEFT);
        ARX_MAP_KEY(ImGuiKey_RightArrow, KEY_RIGHT);
        ARX_MAP_KEY(ImGuiKey_UpArrow, KEY_UP);
        ARX_MAP_KEY(ImGuiKey_DownArrow, KEY_DOWN);
        ARX_MAP_KEY(ImGuiKey_Delete, KEY_DELETE);
        ARX_MAP_KEY(ImGuiKey_Backspace, KEY_BACKSPACE);
        ARX_MAP_KEY(ImGuiKey_Enter, KEY_ENTER);
        ARX_MAP_KEY(ImGuiKey_Escape, KEY_ESCAPE);
        ARX_MAP_KEY(ImGuiKey_Space, KEY_SPACE);
        #undef ARX_MAP_KEY

        // Texto
        int key = GetCharPressed();
        while (key > 0) {
            io.AddInputCharacter((unsigned short)key);
            key = GetCharPressed();
        }

        ImGui::NewFrame();
    }

    // Renderizado eficiente usando el pipeline de rlgl
    static void RenderDrawData(ImDrawData* drawData) {
        rlDrawRenderBatchActive(); // Volcar lo que haya antes
        rlDisableBackfaceCulling();
        rlDisableDepthTest();

        for (int n = 0; n < drawData->CmdListsCount; n++) {
            const ImDrawList* cmdList = drawData->CmdLists[n];
            const ImDrawVert* vtxBuffer = cmdList->VtxBuffer.Data;
            const ImDrawIdx* idxBuffer = cmdList->IdxBuffer.Data;

            for (int cmd_i = 0; cmd_i < cmdList->CmdBuffer.Size; cmd_i++) {
                const ImDrawCmd* pcmd = &cmdList->CmdBuffer[cmd_i];

                if (pcmd->UserCallback) {
                    pcmd->UserCallback(cmdList, pcmd);
                } else {
                    // Scissor Test para recortes de ventanas
                    ImVec2 pos = drawData->DisplayPos;
                    int rectX = (int)(pcmd->ClipRect.x - pos.x);
                    int rectY = (int)(pcmd->ClipRect.y - pos.y);
                    int rectW = (int)(pcmd->ClipRect.z - rectX);
                    int rectH = (int)(pcmd->ClipRect.w - rectY);
                    BeginScissorMode(rectX, rectY, rectW, rectH);

                    // Renderizado de triangulos optimizado
                    for (unsigned int i = 0; i < pcmd->ElemCount; i += 3) {
                        rlBegin(RL_TRIANGLES);

                        // En tu imgui, ImDrawCmd::TextureId ya no existe.
                        // Se obtiene con pcmd->GetTexID().
                        ImTextureID texId = pcmd->GetTexID();
                        rlSetTexture((unsigned int)(intptr_t)texId);

                        ImDrawIdx idx0 = idxBuffer[pcmd->IdxOffset + i];
                        ImDrawIdx idx1 = idxBuffer[pcmd->IdxOffset + i + 1];
                        ImDrawIdx idx2 = idxBuffer[pcmd->IdxOffset + i + 2];

                        auto drawVtx = [](const ImDrawVert& v) {
                            rlColor4ub(v.col & 0xFF, (v.col >> 8) & 0xFF, (v.col >> 16) & 0xFF, (v.col >> 24) & 0xFF);
                            rlTexCoord2f(v.uv.x, v.uv.y);
                            rlVertex2f(v.pos.x, v.pos.y);
                        };

                        drawVtx(vtxBuffer[idx0]);
                        drawVtx(vtxBuffer[idx1]);
                        drawVtx(vtxBuffer[idx2]);
                        rlEnd();
                    }
                }
            }
        }
        EndScissorMode();
        rlEnableDepthTest();
        rlEnableBackfaceCulling();
    }

    static void Render(int screenWidth, int screenHeight) {
        if (!initialized) return;
        ImGui::Render();
        RenderDrawData(ImGui::GetDrawData());
    }

    static void Shutdown() {
        if (!initialized) return;
        UnloadTexture(fontTexture);
        ImGui::DestroyContext(context);
        initialized = false;
    }

    // Helpers utiles
    static bool IsMouseOverImGui() { return initialized && ImGui::GetIO().WantCaptureMouse; }
    static bool WantsKeyboard() { return initialized && ImGui::GetIO().WantCaptureKeyboard; }
};

// =============================================================
// ARXImGuiPanel - Panel para el editor
// =============================================================
class ARXImGuiPanel {
public:
    std::string title = "Panel";
    bool open = true;
    void Draw(std::function<void()> content) {
        if (!open || !ARXImGui::initialized) return;
        if (ImGui::Begin(title.c_str(), &open)) {
            if (content) content();
        }
        ImGui::End();
    }
};

} // namespace ARX

#endif
