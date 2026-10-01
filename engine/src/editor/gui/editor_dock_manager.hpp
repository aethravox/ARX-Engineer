// ==============================================================================
// src/editor/gui/editor_dock_manager.hpp — Gestor de docks (paneles).
//
// ACTUALIZADO: añadido code_editor_ dock.
// ==============================================================================
#pragma once

#include "render/renderer.hpp"

#include <memory>
#include <vector>

namespace arx {

class SceneTreeDock;
class InspectorDock;
class FilesystemDock;
class ViewportPanel;
class OutputLog;
class CodeEditorDock;

class EditorDockManager {
public:
    EditorDockManager();
    ~EditorDockManager();

    void init(Renderer* r);
    void render_all(float delta);

    SceneTreeDock*   scene_tree_dock()   { return scene_tree_.get(); }
    InspectorDock*   inspector_dock()    { return inspector_.get(); }
    FilesystemDock*  filesystem_dock()   { return filesystem_.get(); }
    ViewportPanel*   viewport_dock()     { return viewport_.get(); }
    OutputLog*       output_log_dock()   { return output_log_.get(); }
    CodeEditorDock*  code_editor_dock()  { return code_editor_.get(); }

private:
    std::unique_ptr<SceneTreeDock>   scene_tree_;
    std::unique_ptr<InspectorDock>   inspector_;
    std::unique_ptr<FilesystemDock>  filesystem_;
    std::unique_ptr<ViewportPanel>   viewport_;
    std::unique_ptr<OutputLog>       output_log_;
    std::unique_ptr<CodeEditorDock>  code_editor_;
};

} // namespace arx
