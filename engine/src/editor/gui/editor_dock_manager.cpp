// ==============================================================================
// src/editor/gui/editor_dock_manager.cpp
//
// ACTUALIZADO: añadido code_editor_ dock.
// ==============================================================================
#include "editor_dock_manager.hpp"
#include "scene_tree_dock.hpp"
#include "inspector_dock.hpp"
#include "filesystem_dock.hpp"
#include "viewport.hpp"
#include "output_log.hpp"
#include "code_editor_dock.hpp"

namespace arx {

EditorDockManager::EditorDockManager() = default;
EditorDockManager::~EditorDockManager() = default;

void EditorDockManager::init(Renderer* r) {
    scene_tree_  = std::make_unique<SceneTreeDock>();
    inspector_   = std::make_unique<InspectorDock>();
    filesystem_  = std::make_unique<FilesystemDock>();
    viewport_    = std::make_unique<ViewportPanel>(r);
    output_log_  = std::make_unique<OutputLog>();
    code_editor_ = std::make_unique<CodeEditorDock>();
}

void EditorDockManager::render_all(float delta) {
    scene_tree_->render();
    inspector_ ->render();
    filesystem_->render();
    viewport_  ->render(delta);
    output_log_->render();
    code_editor_->render();
}

} // namespace arx
