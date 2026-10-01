// ==============================================================================
// src/editor/gui/viewport.cpp — Viewport del editor (2D y 3D).
//
// MEJORAS vs versión anterior:
//  • Cámara 3D REAL: el grid y los Voxes se proyectan con yaw/pitch/distance
//    reales de la cámara. Arrastra con el botón DERECHO y el mundo orbita.
//  • Click IZQUIERDO selecciona Voxes (AABB proyectado a pantalla).
//  • Wheel = zoom (en 3D acerca/aleja la cámara).
//  • Botón DERECHO + drag = orbitar (yaw/pitch).
//  • Botón MEDIO + drag = pan (mueve cam_target_).
//  • Reset devuelve la cámara a la posición inicial.
// ==============================================================================
#include "viewport.hpp"
#include "os/os.hpp"
#include "core/types.hpp"
#include "core/logging.hpp"  // ARX_LOG_INFO, ARX_LOG_WARN
#include "icon_manager.hpp"   // ← nuevo: para usar iconos SVG en el viewport
#include "scene_tree_dock.hpp"  // ← para crear Voxes via drag&drop de assets
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "scene/vox.hpp"
#include "scene/2d/vox2d.hpp"
#include "scene/3d/vox3d.hpp"
#include "scene/2d/vox_sprite_2d.hpp"
#include "scene/2d/vox_camera_2d.hpp"
#include "scene/3d/vox_mesh_instance_3d.hpp"
#include "scene/3d/vox_physics_3d.hpp"
#include <imgui.h>
#include <glad/glad.h>
#include <cmath>
#include <functional>
#include <vector>
#include <algorithm>
#include <filesystem>

namespace arx {

// Helper: obtener el nombre del icono SVG dado el class_name del Vox.
// (Definido aquí para no duplicar el de scene_tree_dock.cpp; si se quiere
// reutilizar en otros docks, mover a icon_manager.hpp como función pública.)
static const char* icon_name_for_vox(const std::string& cls) {
    if (cls == "VoxSprite2D")            return "vox_sprite_2d";
    if (cls == "VoxCamera2D")            return "vox_camera_2d";
    if (cls == "VoxCamera3D")            return "vox_camera_3d";
    if (cls == "VoxMeshInstance3D")      return "vox_mesh_3d";
    if (cls == "VoxRigidBody3D")         return "vox_rigid_body_3d";
    if (cls == "VoxStaticBody3D")        return "vox_static_body_3d";
    if (cls == "VoxCharacterBody3D")     return "vox_character_body_3d";
    if (cls == "VoxArea3D")              return "vox_area_3d";
    if (cls == "VoxRayCast3D")           return "vox_ray_cast_3d";
    if (cls == "VoxCollisionShape3D")    return "vox_collision_shape_3d";
    if (cls == "Vox3D")                  return "vox3d";
    if (cls == "Vox2D")                  return "vox2d";
    return "vox";
}

// ==============================================================================
// Helper: crear un Vox según la extensión del archivo que se soltó en el viewport.
// Devuelve nullptr si la extensión no es soportada.
// ==============================================================================
static Vox* create_vox_from_asset(const std::string& path, bool mode_2d) {
    namespace fs = std::filesystem;
    std::string ext = fs::path(path).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (!ext.empty() && ext[0] == '.') ext = ext.substr(1);

    std::string name = fs::path(path).stem().string();

    // Imágenes → Sprite2D (2D) o Vox3D (3D, como placeholder hasta que haya meshes)
    if (ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "bmp" ||
        ext == "tga" || ext == "webp" || ext == "svg") {
        if (mode_2d) {
            auto* s = new VoxSprite2D();
            s->set_name(name.empty() ? "Sprite2D" : name);
            // TODO: cargar textura cuando VoxSprite2D tenga set_texture(path)
            ARX_LOG_INFO("Viewport: creado VoxSprite2D '{}' desde '{}'", s->get_name(), path);
            return s;
        } else {
            auto* v = new Vox3D();
            v->set_name(name.empty() ? "Image3D" : name);
            v->set_position(Vector3(0, 1, 0));
            ARX_LOG_INFO("Viewport: creado Vox3D '{}' desde '{}'", v->get_name(), path);
            return v;
        }
    }
    // Modelos 3D → VoxMeshInstance3D
    if (ext == "obj" || ext == "gltf" || ext == "glb" || ext == "fbx") {
        auto* m = new VoxMeshInstance3D();
        m->set_name(name.empty() ? "Mesh3D" : name);
        m->set_position(Vector3(0, 1, 0));
        ARX_LOG_INFO("Viewport: creado VoxMeshInstance3D '{}' desde '{}'", m->get_name(), path);
        return m;
    }
    // Audio → Vox genérico (todavía no hay VoxAudioStreamPlayer)
    if (ext == "wav" || ext == "ogg" || ext == "mp3" || ext == "flac") {
        auto* v = new Vox();
        v->set_name(name.empty() ? "AudioPlayer" : name);
        ARX_LOG_INFO("Viewport: creado Vox '{}' (audio) desde '{}'", v->get_name(), path);
        return v;
    }
    // .zen → Vox genérico (se cargará como script)
    if (ext == "zen") {
        auto* v = new Vox();
        v->set_name(name.empty() ? "ScriptVox" : name);
        ARX_LOG_INFO("Viewport: creado Vox '{}' (script) desde '{}'", v->get_name(), path);
        return v;
    }

    ARX_LOG_WARN("Viewport: extensión no soportada para drag&drop: '{}'", ext);
    return nullptr;
}

// Estado de cámara 3D persistente (matemática con glm, no renderer)
struct Camera3D {
    float yaw      = 0.4f;
    float pitch    = 0.5f;
    float distance = 10.0f;
    glm::vec3 target{0, 0, 0};
    float fov_deg  = 60.0f;

    glm::vec3 position() const {
        float cp = std::cos(pitch);
        glm::vec3 offset(
            distance * cp * std::sin(yaw),
            distance * std::sin(pitch),
            distance * cp * std::cos(yaw)
        );
        return target + offset;
    }
    glm::mat4 view() const {
        return glm::lookAt(position(), target, glm::vec3(0, 1, 0));
    }
    glm::mat4 proj(float aspect) const {
        return glm::perspective(glm::radians(fov_deg), aspect, 0.1f, 200.0f);
    }
};

static Camera3D s_cam;

// ----------------------------------------------------------------------
// Helper: proyectar clip-space (glm::vec4) a coords de pantalla.
// Requiere clip.w > 0 (punto delante de la cámara).
// ----------------------------------------------------------------------
static ImVec2 project_to_screen(glm::vec4 clip, ImVec2 origin, ImVec2 size) {
    if (clip.w <= 0.1f) return origin;
    float nx = clip.x / clip.w;  // [-1, 1]
    float ny = clip.y / clip.w;
    float sx = origin.x + (nx + 1.0f) * 0.5f * size.x;
    float sy = origin.y + (1.0f - ny) * 0.5f * size.y;
    return ImVec2(sx, sy);
}

// ----------------------------------------------------------------------
// Colores por tipo de Vox (para mantener consistencia con la versión vieja).
// ----------------------------------------------------------------------
static ImU32 color_for_class(const std::string& cls, bool is_2d) {
    if (is_2d) {
        if (cls == "VoxSprite2D")          return IM_COL32(80, 180, 80, 200);
        if (cls == "VoxCamera2D")          return IM_COL32(80, 180, 220, 200);
        if (cls.find("RigidBody") != std::string::npos) return IM_COL32(220, 80, 80, 200);
        if (cls.find("Static")    != std::string::npos) return IM_COL32(150, 150, 150, 200);
        return IM_COL32(107, 54, 217, 200);
    }
    if (cls == "VoxMeshInstance3D")        return IM_COL32(100, 150, 255, 220);
    if (cls == "VoxCamera3D")              return IM_COL32(80, 180, 220, 220);
    if (cls.find("RigidBody") != std::string::npos) return IM_COL32(220, 80, 80, 220);
    if (cls.find("Static")    != std::string::npos) return IM_COL32(150, 150, 150, 220);
    if (cls.find("Character") != std::string::npos) return IM_COL32(80, 220, 120, 220);
    return IM_COL32(150, 100, 220, 220);
}

// ==============================================================================
void ViewportPanel::render(float delta) {
    (void)delta;
    if (!ImGui::Begin("Viewport")) { ImGui::End(); return; }

    render_toolbar();

    ImVec2 size = ImGui::GetContentRegionAvail();
    if (size.x < 1 || size.y < 1) { ImGui::End(); return; }

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 p0 = ImGui::GetCursorScreenPos();
    ImVec2 p1 = ImVec2(p0.x + size.x, p0.y + size.y);

    // Fondo
    dl->AddRectFilled(p0, p1, IM_COL32(12, 12, 18, 255));

    if (mode_2d_) {
        render_2d_(dl, p0, p1, size);
    } else {
        render_3d_(dl, p0, p1, size);
    }

    // Captura de mouse
    ImGui::SetCursorScreenPos(p0);
    ImGui::InvisibleButton("viewport_capture", size);
    handle_input_(p0, p1, size);

    // === Drop target: soltar assets del FileSystem aquí ===
    if (ImGui::BeginDragDropTarget()) {
        if (auto* payload = ImGui::AcceptDragDropPayload("ARX_ASSET_PATH")) {
            const char* path = static_cast<const char*>(payload->Data);
            if (path && path[0]) {
                Vox* new_vox = create_vox_from_asset(path, mode_2d_);
                if (new_vox && root_) {
                    root_->add_child(new_vox);
                    selected_ = new_vox;
                    ARX_LOG_INFO("Viewport: asset soltado y creado como hijo de root: '{}'", path);
                }
            }
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::End();
}

// ==============================================================================
void ViewportPanel::render_2d_(ImDrawList* dl, ImVec2 p0, ImVec2 p1, ImVec2 size) {
    ImVec2 center(p0.x + size.x * 0.5f + pan_offset_.x,
                  p0.y + size.y * 0.5f + pan_offset_.y);
    float scale = zoom_ * 32.0f;

    // Grid menor
    ImU32 grid_minor = IM_COL32(30, 30, 40, 255);
    for (float x = fmodf(center.x, scale); x < p1.x; x += scale)
        dl->AddLine(ImVec2(x, p0.y), ImVec2(x, p1.y), grid_minor);
    for (float x = fmodf(center.x, scale) - scale; x > p0.x; x -= scale)
        dl->AddLine(ImVec2(x, p0.y), ImVec2(x, p1.y), grid_minor);
    for (float y = fmodf(center.y, scale); y < p1.y; y += scale)
        dl->AddLine(ImVec2(p0.x, y), ImVec2(p1.x, y), grid_minor);
    for (float y = fmodf(center.y, scale) - scale; y > p0.y; y -= scale)
        dl->AddLine(ImVec2(p0.x, y), ImVec2(p1.x, y), grid_minor);

    // Ejes
    dl->AddLine(ImVec2(p0.x, center.y), ImVec2(p1.x, center.y), IM_COL32(180, 60, 60, 200), 1.5f);
    dl->AddLine(ImVec2(center.x, p0.y), ImVec2(center.x, p1.y), IM_COL32(60, 180, 60, 200), 1.5f);

    // Voxes 2D
    if (root_) {
        std::function<void(Vox*)> draw = [&](Vox* n) {
            if (n != root_) {
                auto* n2d = dynamic_cast<Vox2D*>(n);
                if (n2d) draw_vox_2d_(n2d, dl, center, scale);
            }
            for (auto* c : n->get_children()) draw(c);
        };
        draw(root_);
    }

    dl->AddText(ImVec2(p0.x + 8, p0.y + 8), IM_COL32(120, 120, 130, 200),
                "2D | Wheel: zoom | Drag-L: pan | Click: select");
}

// ==============================================================================
void ViewportPanel::render_3d_(ImDrawList* dl, ImVec2 p0, ImVec2 p1, ImVec2 size) {
    // Sincronizar cámara del miembro al Camera3D estático
    s_cam.yaw      = cam_yaw_pitch_.x;
    s_cam.pitch    = cam_yaw_pitch_.y;
    s_cam.distance = cam_distance_;
    s_cam.target   = glm::vec3(cam_target_.x, cam_target_.y, cam_target_.z);

    float aspect = size.x / std::max(1.0f, size.y);
    glm::mat4 vp = s_cam.proj(aspect) * s_cam.view();

    // ===== Grid 3D (plano XZ, 20x20 unidades) =====
    struct GridLine { ImVec2 a, b; float depth; ImU32 color; };
    std::vector<GridLine> lines;
    int N = 10;
    for (int i = -N; i <= N; ++i) {
        // Línea paralela a X (varía Z)
        glm::vec4 a = vp * glm::vec4((float)i, 0, (float)-N, 1.0f);
        glm::vec4 b = vp * glm::vec4((float)i, 0, (float) N, 1.0f);
        if (a.w > 0.1f && b.w > 0.1f) {
            ImVec2 sa = project_to_screen(a, p0, size);
            ImVec2 sb = project_to_screen(b, p0, size);
            float d = (a.w + b.w) * 0.5f;
            bool major = (i == 0);
            ImU32 col = major ? IM_COL32(200, 60, 60, 220)
                              : IM_COL32(35, 35, 48, (ImU32)std::max(40, 255 - (int)d * 2));
            lines.push_back({sa, sb, d, col});
        }
        // Línea paralela a Z (varía X)
        glm::vec4 c = vp * glm::vec4((float)-N, 0, (float)i, 1.0f);
        glm::vec4 dd = vp * glm::vec4((float) N, 0, (float)i, 1.0f);
        if (c.w > 0.1f && dd.w > 0.1f) {
            ImVec2 sc = project_to_screen(c, p0, size);
            ImVec2 sd = project_to_screen(dd, p0, size);
            float d = (c.w + dd.w) * 0.5f;
            bool major = (i == 0);
            ImU32 col = major ? IM_COL32(60, 200, 60, 220)
                              : IM_COL32(35, 35, 48, (ImU32)std::max(40, 255 - (int)d * 2));
            lines.push_back({sc, sd, d, col});
        }
    }
    std::sort(lines.begin(), lines.end(), [](const GridLine& a, const GridLine& b){
        return a.depth > b.depth;
    });
    for (auto& l : lines) dl->AddLine(l.a, l.b, l.color, 1.0f);

    // ===== Ejes XYZ en el origen =====
    {
        glm::vec4 o = vp * glm::vec4(0, 0, 0, 1.0f);
        glm::vec4 x = vp * glm::vec4(1.5f, 0, 0, 1.0f);
        glm::vec4 y = vp * glm::vec4(0, 1.5f, 0, 1.0f);
        glm::vec4 z = vp * glm::vec4(0, 0, 1.5f, 1.0f);
        if (o.w > 0.1f && x.w > 0.1f && y.w > 0.1f && z.w > 0.1f) {
            ImVec2 so = project_to_screen(o, p0, size);
            ImVec2 sx = project_to_screen(x, p0, size);
            ImVec2 sy = project_to_screen(y, p0, size);
            ImVec2 sz = project_to_screen(z, p0, size);
            dl->AddLine(so, sx, IM_COL32(220, 70, 70, 255), 2.5f);
            dl->AddLine(so, sy, IM_COL32(70, 220, 70, 255), 2.5f);
            dl->AddLine(so, sz, IM_COL32(70, 110, 220, 255), 2.5f);
            dl->AddText(sx, IM_COL32(220, 70, 70, 255), "X");
            dl->AddText(sy, IM_COL32(70, 220, 70, 255), "Y");
            dl->AddText(sz, IM_COL32(70, 110, 220, 255), "Z");
        }
    }

    // ===== Voxes 3D =====
    struct VoxDraw {
        Vox3D*        vox;
        ImVec2        screen_pos;
        float         screen_size;
        float         depth;
        ImU32         color;
        std::string   name;
    };
    std::vector<VoxDraw> draws;

    if (root_) {
        std::function<void(Vox*)> collect = [&](Vox* n) {
            if (n != root_) {
                auto* n3d = dynamic_cast<Vox3D*>(n);
                if (n3d) {
                    auto pos = n3d->get_position();
                    auto scl = n3d->get_scale();
                    glm::vec4 world(pos.x, pos.y, pos.z, 1.0f);
                    glm::vec4 clip = vp * world;
                    if (clip.w > 0.1f) {
                        ImVec2 sp = project_to_screen(clip, p0, size);
                        float ss = (scl.x + scl.y + scl.z) / 3.0f * 20.0f / clip.w;
                        ss = std::clamp(ss, 4.0f, 200.0f);
                        std::string cls = std::string(n3d->get_class_name());
                        ImU32 color = color_for_class(cls, false);
                        draws.push_back({n3d, sp, ss, clip.w, color, n3d->get_name()});
                    }
                }
            }
            for (auto* c : n->get_children()) collect(c);
        };
        collect(root_);
    }

    // Painter's algorithm: lejanos primero
    std::sort(draws.begin(), draws.end(), [](const VoxDraw& a, const VoxDraw& b){
        return a.depth > b.depth;
    });

    for (auto& d : draws) {
        draw_vox_3d_real_(d.vox, dl, d.screen_pos, d.screen_size, d.color, d.name);
    }

    // === Gizmo 3D sobre el Vox seleccionado ===
    gizmo_hover_axis_ = draw_gizmo_3d_(dl, p0, size, vp);

    // Texto info
    char info[256];
    const char* mode_str = (gizmo_mode_ == GizmoMode::Move) ? "MOVE" :
                           (gizmo_mode_ == GizmoMode::Rotate) ? "ROTATE" : "SCALE";
    std::snprintf(info, sizeof(info),
                  "3D [%s] | yaw: %.0f° pitch: %.0f° dist: %.1f | Drag-R: orbit | Wheel: zoom | Click: select | W/E/R: gizmo",
                  mode_str, s_cam.yaw * 57.2958f, s_cam.pitch * 57.2958f, s_cam.distance);
    dl->AddText(ImVec2(p0.x + 8, p0.y + 8), IM_COL32(120, 120, 130, 200), info);
}

// ==============================================================================
void ViewportPanel::draw_vox_3d_real_(Vox3D* n, ImDrawList* dl,
                                       ImVec2 sp, float ss, ImU32 color,
                                       const std::string& name) {
    // sp = centro en pantalla, ss = "radio" en píxeles
    ImVec2 p0(sp.x - ss, sp.y - ss);
    ImVec2 p1(sp.x + ss, sp.y + ss);

    // === Sombra en el "piso" (debajo del icono) ===
    dl->AddRectFilled(
        ImVec2(sp.x - ss * 0.7f, sp.y + ss + 2),
        ImVec2(sp.x + ss * 0.7f, sp.y + ss + 5),
        IM_COL32(0, 0, 0, 100));

    // === Icono SVG del tipo de Vox ===
    // En lugar de dibujar un cuadrado de color sólido, usamos el icono SVG
    // correspondiente al tipo de Vox. Esto hace que en el viewport 3D se vea
    // claramente qué tipo de Vox es (cámara, rigidbody, mesh, etc.).
    std::string cls = std::string(n->get_class_name());
    const char* icon_name = icon_name_for_vox(cls);
    ImTextureID tex = IconManager::get().imgui_texture(icon_name);

    if (tex != 0) {
        // Dibujar el icono SVG escalado al tamaño del Vox en pantalla
        dl->AddImage(tex, p0, p1);
    } else {
        // Fallback: cuadrado de color sólido si el icono no cargó
        dl->AddRectFilled(p0, p1, color);
    }

    // === Borde del icono ===
    dl->AddRect(p0, p1, IM_COL32(255, 255, 255, 120));

    // === Selection highlight (borde cyan) ===
    if (selected_ == n) {
        dl->AddRect(ImVec2(p0.x - 3, p0.y - 3),
                    ImVec2(p1.x + 3, p1.y + 3),
                    IM_COL32(26, 217, 235, 255), 0, 0, 2.5f);
    }

    // === Nombre debajo del icono ===
    dl->AddText(ImVec2(p0.x, p1.y + 6),
                IM_COL32(220, 220, 230, 240), name.c_str());
}

// ==============================================================================
void ViewportPanel::draw_vox_2d_(Vox2D* n, ImDrawList* dl, ImVec2 origin, float scale) {
    auto pos = n->get_position();
    auto scl = n->get_scale();

    ImVec2 screen_pos(origin.x + pos.x * scale, origin.y - pos.y * scale);
    float w = 32.0f * scl.x;
    float h = 32.0f * scl.y;
    ImVec2 p0(screen_pos.x - w * 0.5f, screen_pos.y - h * 0.5f);
    ImVec2 p1(screen_pos.x + w * 0.5f, screen_pos.y + h * 0.5f);

    std::string cls = std::string(n->get_class_name());

    // === Icono SVG del tipo de Vox 2D ===
    const char* icon_name = icon_name_for_vox(cls);
    ImTextureID tex = IconManager::get().imgui_texture(icon_name);

    if (tex != 0) {
        dl->AddImage(tex, p0, p1);
    } else {
        // Fallback: cuadrado de color sólido
        ImU32 color = color_for_class(cls, true);
        dl->AddRectFilled(p0, p1, color);
    }

    // Borde
    dl->AddRect(p0, p1, IM_COL32(255, 255, 255, 120));

    if (selected_ == n) {
        dl->AddRect(ImVec2(p0.x - 2, p0.y - 2), ImVec2(p1.x + 2, p1.y + 2),
                    IM_COL32(26, 217, 235, 255), 0, 0, 2.0f);
    }

    dl->AddText(ImVec2(p0.x, p1.y + 2), IM_COL32(220, 220, 230, 220),
                n->get_name().c_str());

    for (auto* c : n->get_children()) {
        auto* c2d = dynamic_cast<Vox2D*>(c);
        if (c2d) draw_vox_2d_(c2d, dl, origin, scale);
    }
}

// Compat con la firma vieja (no se usa, evita link errors si algún .cpp la llama)
void ViewportPanel::draw_vox_3d_(Vox3D* n, ImDrawList* dl, float cx, float horizon, float scale) {
    (void)n; (void)dl; (void)cx; (void)horizon; (void)scale;
}

// ==============================================================================
void ViewportPanel::handle_input_(ImVec2 p0, ImVec2 p1, ImVec2 size) {
    (void)p1;
    if (!ImGui::IsItemHovered() && !ImGui::IsItemActive()) return;

    ImGuiIO& io = ImGui::GetIO();

    // ===== Wheel = zoom =====
    if (io.MouseWheel != 0) {
        if (mode_2d_) {
            zoom_ *= (1.0f + io.MouseWheel * 0.1f);
            zoom_ = std::clamp(zoom_, 0.1f, 20.0f);
        } else {
            cam_distance_ *= (1.0f - io.MouseWheel * 0.1f);
            cam_distance_ = std::clamp(cam_distance_, 1.5f, 80.0f);
        }
    }

    // ===== Botón DERECHO = orbitar (3D) o pan (2D) =====
    if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
        ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
        ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);
        if (mode_2d_) {
            pan_offset_.x += delta.x;
            pan_offset_.y += delta.y;
        } else {
            // Cámara orbit: izquierda → yaw aumenta, arriba → pitch aumenta
            // (antes estaba invertido)
            cam_yaw_pitch_.x -= delta.x * 0.01f;
            cam_yaw_pitch_.y += delta.y * 0.01f;
            cam_yaw_pitch_.y = std::clamp(cam_yaw_pitch_.y, -1.55f, 1.55f);
        }
    }

    // ===== Botón MEDIO = pan del target (3D) =====
    if (ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {
        ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle);
        ImGui::ResetMouseDragDelta(ImGuiMouseButton_Middle);
        if (mode_2d_) {
            pan_offset_.x += delta.x;
            pan_offset_.y += delta.y;
        } else {
            float f = cam_distance_ * 0.002f;
            // Ejes de la cámara en el plano horizontal/vertical
            float cp = std::cos(cam_yaw_pitch_.y);
            glm::vec3 right( std::cos(cam_yaw_pitch_.x), 0, -std::sin(cam_yaw_pitch_.x));
            glm::vec3 up(   -std::sin(cam_yaw_pitch_.y) * std::sin(cam_yaw_pitch_.x),
                             std::cos(cam_yaw_pitch_.y),
                            -std::sin(cam_yaw_pitch_.y) * std::cos(cam_yaw_pitch_.x));
            // Pan: derecha → target se mueve a la izquierda (para que el mundo se mueva a la derecha)
            //       arriba → target se mueve abajo (para que el mundo suba)
            cam_target_.x += right.x * delta.x * f + up.x * delta.y * f;
            cam_target_.y += right.y * delta.x * f + up.y * delta.y * f;
            cam_target_.z += right.z * delta.x * f + up.z * delta.y * f;
        }
    }

    // ===== Click IZQUIERDO = seleccionar Vox O iniciar drag de gizmo =====
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
        ImVec2 mp = ImGui::GetMousePos();
        // 1. Primero intentar con el gizmo (si hay Vox seleccionado en 3D)
        if (!mode_2d_ && selected_) {
            // Reconstruir VP
            s_cam.yaw = cam_yaw_pitch_.x;
            s_cam.pitch = cam_yaw_pitch_.y;
            s_cam.distance = cam_distance_;
            s_cam.target = glm::vec3(cam_target_.x, cam_target_.y, cam_target_.z);
            float aspect = size.x / std::max(1.0f, size.y);
            glm::mat4 vp = s_cam.proj(aspect) * s_cam.view();

            if (handle_gizmo_input_(mp, p0, size, vp)) {
                // El gizmo capturó el click — no seleccionar nada más.
                goto input_done;
            }
        }
        // 2. Si el gizmo no lo capturó, seleccionar Vox
        {
            Vox* hit = pick_vox_at_(mp, p0, size);
            if (hit) selected_ = hit;
        }
        input_done:;
    }

    // ===== Drag del gizmo en curso =====
    if (gizmo_dragging_ && !mode_2d_ && selected_) {
        ImGuiIO& io2 = ImGui::GetIO();
        // Si se soltó el botón, terminar drag
        if (!io2.MouseDown[0]) {
            gizmo_dragging_ = false;
            gizmo_active_axis_ = -1;
        } else {
            // Reconstruir VP y aplicar drag
            s_cam.yaw = cam_yaw_pitch_.x;
            s_cam.pitch = cam_yaw_pitch_.y;
            s_cam.distance = cam_distance_;
            s_cam.target = glm::vec3(cam_target_.x, cam_target_.y, cam_target_.z);
            float aspect = size.x / std::max(1.0f, size.y);
            glm::mat4 vp = s_cam.proj(aspect) * s_cam.view();
            handle_gizmo_input_(io2.MousePos, p0, size, vp);
        }
    }
}

// ==============================================================================
Vox* ViewportPanel::pick_vox_at_(ImVec2 mouse, ImVec2 origin, ImVec2 size) {
    if (!root_) return nullptr;

    s_cam.yaw      = cam_yaw_pitch_.x;
    s_cam.pitch    = cam_yaw_pitch_.y;
    s_cam.distance = cam_distance_;
    s_cam.target   = glm::vec3(cam_target_.x, cam_target_.y, cam_target_.z);
    float aspect = size.x / std::max(1.0f, size.y);
    glm::mat4 vp = s_cam.proj(aspect) * s_cam.view();

    Vox* best = nullptr;
    float best_w = 1e9f;

    std::function<void(Vox*)> visit = [&](Vox* n) {
        if (n != root_) {
            auto* n3d = dynamic_cast<Vox3D*>(n);
            if (n3d) {
                auto pos = n3d->get_position();
                auto scl = n3d->get_scale();
                glm::vec4 world(pos.x, pos.y, pos.z, 1.0f);
                glm::vec4 clip = vp * world;
                if (clip.w > 0.1f) {
                    ImVec2 sp = project_to_screen(clip, origin, size);
                    float ss = (scl.x + scl.y + scl.z) / 3.0f * 20.0f / clip.w;
                    ss = std::clamp(ss, 4.0f, 200.0f);
                    if (mouse.x >= sp.x - ss && mouse.x <= sp.x + ss &&
                        mouse.y >= sp.y - ss && mouse.y <= sp.y + ss) {
                        if (clip.w < best_w) {
                            best_w = clip.w;
                            best = n3d;
                        }
                    }
                }
            }
        }
        for (auto* c : n->get_children()) visit(c);
    };
    visit(root_);
    return best;
}

// Compat: handle_input_ viejo sin args (no se usa)
void ViewportPanel::handle_input_() {}

// ==============================================================================
void ViewportPanel::render_toolbar() {
    if (ImGui::Button(mode_2d_ ? "2D" : "3D", ImVec2(40, 0))) mode_2d_ = !mode_2d_;
    ImGui::SameLine();
    if (ImGui::Button("Reset", ImVec2(50, 0))) {
        pan_offset_ = {0, 0}; zoom_ = 1.0f;
        cam_distance_  = 10.0f;
        cam_yaw_pitch_ = {0.4f, 0.5f};
        cam_target_    = {0, 0, 0};
    }
    ImGui::SameLine(); ImGui::TextDisabled("|");
    ImGui::SameLine();
    if (ImGui::Button(playing_ ? "⏸" : "▶", ImVec2(30, 0))) playing_ = !playing_;
    ImGui::SameLine(); ImGui::Button("⏹", ImVec2(30, 0));
    ImGui::SameLine(); ImGui::Button("⏭", ImVec2(30, 0));
    ImGui::SameLine(); ImGui::TextDisabled("|");
    ImGui::SameLine();
    // === Gizmo mode buttons ===
    if (!mode_2d_) {
        ImGui::PushStyleColor(ImGuiCol_Button, gizmo_mode_ == GizmoMode::Move ?
                            ImVec4(0.49f, 0.23f, 0.93f, 0.7f) : ImGui::GetStyle().Colors[ImGuiCol_Button]);
        if (ImGui::Button("Move [W]", ImVec2(70, 0))) gizmo_mode_ = GizmoMode::Move;
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, gizmo_mode_ == GizmoMode::Rotate ?
                            ImVec4(0.49f, 0.23f, 0.93f, 0.7f) : ImGui::GetStyle().Colors[ImGuiCol_Button]);
        if (ImGui::Button("Rotate [E]", ImVec2(80, 0))) gizmo_mode_ = GizmoMode::Rotate;
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, gizmo_mode_ == GizmoMode::Scale ?
                            ImVec4(0.49f, 0.23f, 0.93f, 0.7f) : ImGui::GetStyle().Colors[ImGuiCol_Button]);
        if (ImGui::Button("Scale [R]", ImVec2(75, 0))) gizmo_mode_ = GizmoMode::Scale;
        ImGui::PopStyleColor();
        ImGui::SameLine();
    }
    if (mode_2d_) ImGui::TextDisabled("Zoom: %.1fx", zoom_);
    else          ImGui::TextDisabled("Dist: %.1f", cam_distance_);
    ImGui::Separator();

    // === Hotkeys W/E/R para cambiar modo gizmo (solo si no estamos editando texto) ===
    if (!ImGui::GetIO().WantTextInput && !mode_2d_) {
        if (ImGui::IsKeyPressed(ImGuiKey_W)) gizmo_mode_ = GizmoMode::Move;
        if (ImGui::IsKeyPressed(ImGuiKey_E)) gizmo_mode_ = GizmoMode::Rotate;
        if (ImGui::IsKeyPressed(ImGuiKey_R)) gizmo_mode_ = GizmoMode::Scale;
    }
}

// ==============================================================================
// Dibuja el gizmo 3D sobre el Vox seleccionado.
// Devuelve el índice del eje bajo el mouse (-1 si ninguno).
// ==============================================================================
int ViewportPanel::draw_gizmo_3d_(ImDrawList* dl, ImVec2 origin, ImVec2 size, const glm::mat4& vp) {
    if (!selected_) return -1;

    auto* n3d = dynamic_cast<Vox3D*>(selected_);
    if (!n3d) return -1;

    auto pos = n3d->get_position();
    glm::vec4 world_origin(pos.x, pos.y, pos.z, 1.0f);
    glm::vec4 clip_origin = vp * world_origin;
    if (clip_origin.w <= 0.1f) return -1;
    ImVec2 screen_origin = project_to_screen(clip_origin, origin, size);

    // Tamaño del gizmo en pantalla (constante, sin importar zoom)
    float gizmo_len_px = 60.0f;

    // Proyectar los extremos de cada eje (origen + unitario * escala_mundo)
    // La escala_mundo se elige para que el eje mida gizmo_len_px en pantalla.
    // Para eso proyectamos origen y origen+eje (con escala 1) y medimos el largo en px.
    struct Axis { const char* name; glm::vec3 dir; ImU32 color; ImVec2 screen_end; float length_px; };
    Axis axes[3] = {
        {"X", glm::vec3(1, 0, 0), IM_COL32(220, 70, 70, 255), ImVec2(0,0), 0},
        {"Y", glm::vec3(0, 1, 0), IM_COL32(70, 220, 70, 255), ImVec2(0,0), 0},
        {"Z", glm::vec3(0, 0, 1), IM_COL32(70, 110, 220, 255), ImVec2(0,0), 0},
    };

    // Calcular extremos proyectados y largo en píxeles
    for (int i = 0; i < 3; ++i) {
        glm::vec4 world_end = glm::vec4(pos.x + axes[i].dir.x,
                                        pos.y + axes[i].dir.y,
                                        pos.z + axes[i].dir.z, 1.0f);
        glm::vec4 clip_end = vp * world_end;
        if (clip_end.w > 0.1f) {
            ImVec2 screen_end = project_to_screen(clip_end, origin, size);
            float dx = screen_end.x - screen_origin.x;
            float dy = screen_end.y - screen_origin.y;
            float len = std::sqrt(dx*dx + dy*dy);
            axes[i].length_px = len;
            // Re-escalar el extremo para que mida gizmo_len_px
            if (len > 0.001f) {
                float k = gizmo_len_px / len;
                axes[i].screen_end = ImVec2(screen_origin.x + dx * k,
                                             screen_origin.y + dy * k);
            } else {
                axes[i].screen_end = screen_origin;
            }
        } else {
            axes[i].length_px = 0;
            axes[i].screen_end = screen_origin;
        }
    }

    // Hit-testing: calcular distancia del mouse a cada línea
    ImVec2 mouse = ImGui::GetMousePos();
    int hover_axis = -1;
    float best_dist = 8.0f;  // threshold en píxeles
    for (int i = 0; i < 3; ++i) {
        // Distancia punto → segmento
        ImVec2 a = screen_origin;
        ImVec2 b = axes[i].screen_end;
        float dx = b.x - a.x, dy = b.y - a.y;
        float len_sq = dx*dx + dy*dy;
        if (len_sq < 0.001f) continue;
        float t = ((mouse.x - a.x) * dx + (mouse.y - a.y) * dy) / len_sq;
        t = std::clamp(t, 0.0f, 1.0f);
        float px = a.x + t * dx;
        float py = a.y + t * dy;
        float dist = std::sqrt((mouse.x - px)*(mouse.x - px) + (mouse.y - py)*(mouse.y - py));
        if (dist < best_dist) {
            best_dist = dist;
            hover_axis = i;
        }
    }

    // === Dibujar los 3 ejes ===
    for (int i = 0; i < 3; ++i) {
        ImU32 color = axes[i].color;
        float thickness = 2.0f;
        // Resaltar si está hover o siendo arrastrado
        if (i == hover_axis || i == gizmo_active_axis_) {
            color = IM_COL32(255, 255, 100, 255);  // amarillo brillante
            thickness = 4.0f;
        }
        // Línea del eje
        dl->AddLine(screen_origin, axes[i].screen_end, color, thickness);
        // Flecha (triángulo) en el extremo
        ImVec2 dir = ImVec2(axes[i].screen_end.x - screen_origin.x,
                            axes[i].screen_end.y - screen_origin.y);
        float dir_len = std::sqrt(dir.x*dir.x + dir.y*dir.y);
        if (dir_len > 0.001f) {
            ImVec2 dir_n = ImVec2(dir.x / dir_len, dir.y / dir_len);
            ImVec2 perp = ImVec2(-dir_n.y, dir_n.x);
            float arrow_size = 8.0f;
            ImVec2 tip = axes[i].screen_end;
            ImVec2 base1(tip.x - dir_n.x * arrow_size + perp.x * arrow_size * 0.5f,
                         tip.y - dir_n.y * arrow_size + perp.y * arrow_size * 0.5f);
            ImVec2 base2(tip.x - dir_n.x * arrow_size - perp.x * arrow_size * 0.5f,
                         tip.y - dir_n.y * arrow_size - perp.y * arrow_size * 0.5f);
            dl->AddTriangleFilled(tip, base1, base2, color);
        }
        // Etiqueta del eje
        ImVec2 label_pos(axes[i].screen_end.x + dir.x * 0.15f,
                         axes[i].screen_end.y + dir.y * 0.15f);
        dl->AddText(label_pos, color, axes[i].name);
    }

    // Cuadrado central en el origen (para indicar selección activa)
    dl->AddRectFilled(
        ImVec2(screen_origin.x - 3, screen_origin.y - 3),
        ImVec2(screen_origin.x + 3, screen_origin.y + 3),
        IM_COL32(255, 255, 255, 200));

    return hover_axis;
}

// ==============================================================================
// Procesa input del gizmo. Devuelve true si el evento fue consumido.
// ==============================================================================
bool ViewportPanel::handle_gizmo_input_(ImVec2 mouse, ImVec2 origin, ImVec2 size, const glm::mat4& vp) {
    if (!selected_) return false;
    auto* n3d = dynamic_cast<Vox3D*>(selected_);
    if (!n3d) return false;

    // === Iniciar drag ===
    if (!gizmo_dragging_ && gizmo_hover_axis_ >= 0) {
        // Empezar a arrastrar el eje hover
        gizmo_dragging_ = true;
        gizmo_active_axis_ = gizmo_hover_axis_;
        drag_start_vox_pos_ = n3d->get_position();
        drag_start_vox_rot_ = n3d->get_rotation();
        drag_start_vox_scl_ = n3d->get_scale();
        drag_start_mouse_ = mouse;
        return true;
    }

    // === Aplicar drag en curso ===
    if (gizmo_dragging_ && gizmo_active_axis_ >= 0) {
        ImVec2 mouse_delta(mouse.x - drag_start_mouse_.x,
                           mouse.y - drag_start_mouse_.y);

        // Vectores unitarios de los ejes del mundo en screen-space
        auto pos = drag_start_vox_pos_;
        glm::vec4 world_origin(pos.x, pos.y, pos.z, 1.0f);
        glm::vec4 clip_origin = vp * world_origin;
        if (clip_origin.w <= 0.1f) return true;
        ImVec2 screen_origin = project_to_screen(clip_origin, origin, size);

        // Proyectar extremos de los 3 ejes para tener direcciones en pantalla
        glm::vec3 world_axes[3] = {
            glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)
        };
        ImVec2 screen_dirs[3];
        for (int i = 0; i < 3; ++i) {
            glm::vec4 world_end(pos.x + world_axes[i].x,
                                pos.y + world_axes[i].y,
                                pos.z + world_axes[i].z, 1.0f);
            glm::vec4 clip_end = vp * world_end;
            if (clip_end.w > 0.1f) {
                ImVec2 se = project_to_screen(clip_end, origin, size);
                ImVec2 dir(se.x - screen_origin.x, se.y - screen_origin.y);
                float len = std::sqrt(dir.x*dir.x + dir.y*dir.y);
                if (len > 0.001f) {
                    screen_dirs[i] = ImVec2(dir.x / len, dir.y / len);
                } else {
                    screen_dirs[i] = ImVec2(0, 0);
                }
            } else {
                screen_dirs[i] = ImVec2(0, 0);
            }
        }

        // Proyección del mouse_delta sobre el eje activo en pantalla
        ImVec2 axis_dir = screen_dirs[gizmo_active_axis_];
        float proj = mouse_delta.x * axis_dir.x + mouse_delta.y * axis_dir.y;

        // Convertir proyección en píxeles a unidades del mundo.
        // Aproximación: 1 unidad del mundo = screen_dirs[i].length_px en píxeles.
        // Pero screen_dirs[i] está normalizado, así que hay que re-calcular length_px.
        // Mejor: la escala píxel/mundo se puede calcular sabiendo que un vector
        // de 1 unidad del mundo se proyecta a screen_dirs[i] * length_px.
        // Calculamos length_px ahora:
        glm::vec4 we_x = vp * glm::vec4(pos.x + 1, pos.y, pos.z, 1.0f);
        glm::vec4 we_y = vp * glm::vec4(pos.x, pos.y + 1, pos.z, 1.0f);
        glm::vec4 we_z = vp * glm::vec4(pos.x, pos.y, pos.z + 1, 1.0f);
        float length_px[3] = {0, 0, 0};
        if (we_x.w > 0.1f) {
            ImVec2 se = project_to_screen(we_x, origin, size);
            float dx = se.x - screen_origin.x, dy = se.y - screen_origin.y;
            length_px[0] = std::sqrt(dx*dx + dy*dy);
        }
        if (we_y.w > 0.1f) {
            ImVec2 se = project_to_screen(we_y, origin, size);
            float dx = se.x - screen_origin.x, dy = se.y - screen_origin.y;
            length_px[1] = std::sqrt(dx*dx + dy*dy);
        }
        if (we_z.w > 0.1f) {
            ImVec2 se = project_to_screen(we_z, origin, size);
            float dx = se.x - screen_origin.x, dy = se.y - screen_origin.y;
            length_px[2] = std::sqrt(dx*dx + dy*dy);
        }

        float world_per_px = (length_px[gizmo_active_axis_] > 0.001f)
                              ? (1.0f / length_px[gizmo_active_axis_])
                              : 0.01f;
        float world_delta = proj * world_per_px;

        // Aplicar según modo
        if (gizmo_mode_ == GizmoMode::Move) {
            Vector3 new_pos = drag_start_vox_pos_;
            if (gizmo_active_axis_ == 0) new_pos.x += world_delta;
            else if (gizmo_active_axis_ == 1) new_pos.y += world_delta;
            else if (gizmo_active_axis_ == 2) new_pos.z += world_delta;
            n3d->set_position(new_pos);
        } else if (gizmo_mode_ == GizmoMode::Rotate) {
            // Rotación: 1 px = 1 grado (aprox)
            float angle_deg = proj * 1.0f;
            float angle_rad = angle_deg * (3.14159265f / 180.0f);
            Vector3 new_rot = drag_start_vox_rot_;
            if (gizmo_active_axis_ == 0) new_rot.x += angle_rad;
            else if (gizmo_active_axis_ == 1) new_rot.y += angle_rad;
            else if (gizmo_active_axis_ == 2) new_rot.z += angle_rad;
            n3d->set_rotation(new_rot);
        } else if (gizmo_mode_ == GizmoMode::Scale) {
            // Scale: delta proporcional
            float scale_delta = world_delta * 0.5f;
            Vector3 new_scl = drag_start_vox_scl_;
            if (gizmo_active_axis_ == 0) new_scl.x = std::max(0.01f, new_scl.x + scale_delta);
            else if (gizmo_active_axis_ == 1) new_scl.y = std::max(0.01f, new_scl.y + scale_delta);
            else if (gizmo_active_axis_ == 2) new_scl.z = std::max(0.01f, new_scl.z + scale_delta);
            n3d->set_scale(new_scl);
        }

        return true;
    }

    return false;
}

} // namespace arx
