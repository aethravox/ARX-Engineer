// ==============================================================================
// src/scene/animation/skeletal_animation.cpp
// ==============================================================================
#include "skeletal_animation.hpp"
#include "core/logging.hpp"
#include "core/math.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

namespace arx {

// ===================== AnimationTree ========================================
void AnimationTree::add_animation(const SkeletalAnimation& anim) {
    anims_[anim.name] = anim;
}

void AnimationTree::add_state(const State& s) {
    states_[s.name] = s;
    if (current_state_.empty()) current_state_ = s.name;
}

void AnimationTree::transition_to(const std::string& state_name, float blend_time) {
    if (!states_.count(state_name)) return;
    if (current_state_ == state_name) return;
    next_state_      = state_name;
    blend_remaining_ = blend_time;
    blend_time_      = blend_time;
}

void AnimationTree::process(float delta) {
    if (!skeleton_ || current_state_.empty()) return;

    position_ += delta;

    auto it = states_.find(current_state_);
    if (it == states_.end()) return;

    auto anim_it = anims_.find(it->second.animation);
    if (anim_it == anims_.end()) return;

    float weight = 1.0f;
    if (!next_state_.empty() && blend_remaining_ > 0.0f) {
        blend_remaining_ -= delta;
        weight = blend_remaining_ / blend_time_;
        // Aplicar next_state con peso (1 - weight).
        auto next_it = states_.find(next_state_);
        if (next_it != states_.end()) {
            auto na = anims_.find(next_it->second.animation);
            if (na != anims_.end()) {
                apply_animation(na->second, position_, 1.0f - weight);
            }
        }
        if (blend_remaining_ <= 0.0f) {
            current_state_ = next_state_;
            next_state_.clear();
            position_ = 0.0f;
        }
    }

    apply_animation(anim_it->second, position_, weight);
}

void AnimationTree::apply_animation(const SkeletalAnimation& anim, float time, float weight) {
    if (!skeleton_) return;
    for (const auto& track : anim.tracks) {
        if (track.bone_index < 0 || track.bone_index >= skeleton_->get_bone_count())
            continue;
        // Buscar keyframes.
        Vector3 pos{0,0,0}, scale{1,1,1};
        Quaternion rot{1,0,0,0};

        if (!track.position_keys.empty()) {
            // Interpolar.
            const auto& keys = track.position_keys;
            if (time <= keys.front().time) pos = keys.front().value.to_vector3();
            else if (time >= keys.back().time) pos = keys.back().value.to_vector3();
            else {
                for (size_t i = 0; i < keys.size() - 1; ++i) {
                    if (time >= keys[i].time && time <= keys[i+1].time) {
                        float t = (time - keys[i].time) /
                                  std::max(keys[i+1].time - keys[i].time, 1e-6f);
                        pos = math::lerp(keys[i].value.to_vector3(),
                                          keys[i+1].value.to_vector3(), t);
                        break;
                    }
                }
            }
        }
        // Aplicar al bone.
        Transform3D pose;
        pose.origin = pos;
        pose.basis  = glm::mat3_cast(rot);
        skeleton_->set_bone_pose(track.bone_index, pose);
    }
}

// ===================== SkinnedMeshInstance3D ================================
void SkinnedMeshInstance3D::process(float /*delta*/) {
    if (!skeleton_) return;
    skin_palette_.resize(skeleton_->get_bone_count());
    for (int i = 0; i < skeleton_->get_bone_count(); ++i) {
        const auto& bone = skeleton_->get_bone(i);
        // final = bone.rest^-1 * bone.pose * parent_chain
        // Simplificación: solo usar pose.
        skin_palette_[i] = bone.pose.to_matrix4();
    }
}

// ===================== IKChain (FABRIK) =====================================
void IKChain::solve(VoxSkeleton3D* skeleton) {
    if (!skeleton || chain_.empty()) return;

    // FABRIK: Forward And Backward Reaching Inverse Kinematics.
    // 1. Recoger posiciones actuales de los bones.
    std::vector<Vector3> positions;
    for (int idx : chain_) {
        if (idx >= 0 && idx < skeleton->get_bone_count())
            positions.push_back(skeleton->get_bone(idx).pose.origin);
    }
    if (positions.empty()) return;

    Vector3 root = positions.front();

    for (int iter = 0; iter < iterations_; ++iter) {
        // Backward: del end-effector al root.
        positions.back() = target_;
        for (int i = (int)positions.size() - 2; i >= 0; --i) {
            Vector3 dir = glm::normalize(positions[i] - positions[i+1]);
            positions[i] = positions[i+1] + dir * 1.0f;  // 1.0f = bone_length
        }

        // Forward: del root al end-effector.
        positions.front() = root;
        for (int i = 1; i < (int)positions.size(); ++i) {
            Vector3 dir = glm::normalize(positions[i] - positions[i-1]);
            positions[i] = positions[i-1] + dir * 1.0f;
        }

        // Check tolerancia.
        if (glm::distance(positions.back(), target_) < tolerance_) break;
    }

    // Escribir de vuelta al skeleton.
    for (size_t i = 0; i < positions.size() && i < chain_.size(); ++i) {
        Transform3D pose;
        pose.origin = positions[i];
        skeleton->set_bone_pose(chain_[i], pose);
    }
}

} // namespace arx
