// ==============================================================================
// src/scene/3d/extra_nodes_3d.cpp
// ==============================================================================
#include "vox_extra_3d.hpp"

namespace arx {

void VoxRemoteTransform3D::process(float /*delta*/) {
    if (remote_path_.empty()) return;
    Vox* target = get_node(remote_path_);
    if (!target) return;
    auto* t3d = dynamic_cast<Vox3D*>(target);
    if (!t3d) return;
    Transform3D t = get_global_transform();
    if (update_position_) t3d->set_position(t.origin);
    // Simplificación: rotation/scale no se copian individualmente en este stub.
}

void VoxVisibleOnScreenNotifier3D::process(float /*delta*/) {
    // En una impl completa se compararía el AABB contra el frustum de la cámara.
    bool was = on_screen_;
    on_screen_ = true;  // Stub: siempre true.
    if (!was && on_screen_) {
        emit_signal(sid("screen_entered"), {});
    } else if (was && !on_screen_) {
        emit_signal(sid("screen_exited"), {});
    }
}

int VoxSkeleton3D::add_bone(const std::string& name, int parent) {
    Bone b; b.name = name; b.parent = parent;
    bones_.push_back(std::move(b));
    return (int)bones_.size() - 1;
}

int VoxSkeleton3D::find_bone(const std::string& name) const {
    for (size_t i = 0; i < bones_.size(); ++i)
        if (bones_[i].name == name) return (int)i;
    return -1;
}

void VoxSkeleton3D::set_bone_pose(int idx, const Transform3D& p) {
    if (idx >= 0 && idx < (int)bones_.size()) bones_[idx].pose = p;
}

} // namespace arx
