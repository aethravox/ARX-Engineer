// ==============================================================================
// src/scene/animation/skeletal_animation.hpp — Animación esquelética.
// Skeleton + bones + skinning + blend trees.
// ==============================================================================
#pragma once

#include "scene/animation/animation.hpp"
#include "scene/3d/vox_extra_3d.hpp"
#include "render/texture.hpp"

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

namespace arx {

// VoxSkeleton3D extendido (en extra_nodes_3d.hpp está la versión básica).
// Aquí añadimos animación con skinning.

// BoneAnimation — keyframes de position/rotation/scale por bone.
struct BoneTrack {
    std::string bone_name;
    int         bone_index = -1;
    std::vector<VoxAnimationPlayer::Keyframe> position_keys;
    std::vector<VoxAnimationPlayer::Keyframe> rotation_keys;
    std::vector<VoxAnimationPlayer::Keyframe> scale_keys;
};

// SkeletalAnimation — animación con tracks por bone.
struct SkeletalAnimation {
    std::string name;
    float       length = 1.0f;
    bool        loop   = false;
    std::vector<BoneTrack> tracks;
};

// AnimationTree — blend tree para mezclar animaciones (idle/walk/run).
class AnimationTree : public Vox {
public:
    ARX_CLASS(AnimationTree, Vox);
public:

    enum class BlendMode { Linear, Additive, Masked };

    struct BlendSpace1D {
        std::vector<std::pair<std::string, float>> anims;  // anim, speed
        float blend_position = 0.0f;
    };

    struct State {
        std::string name;
        std::string animation;
        float       speed = 1.0f;
        bool        looping = true;
        std::unordered_map<std::string, std::pair<std::string, float>> transitions;
    };

    void add_animation(const SkeletalAnimation& anim);
    void add_state(const State& s);
    void transition_to(const std::string& state_name, float blend_time = 0.2f);

    void set_blend_position(float pos) { blend_position_ = pos; }
    void set_skeleton(VoxSkeleton3D* s)   { skeleton_ = s; }

    void process(float delta) override;

private:
    std::unordered_map<std::string, SkeletalAnimation> anims_;
    std::unordered_map<std::string, State>             states_;
    VoxSkeleton3D* skeleton_ = nullptr;
    std::string current_state_;
    std::string next_state_;
    float       blend_time_      = 0.0f;
    float       blend_remaining_ = 0.0f;
    float       position_        = 0.0f;
    float       blend_position_  = 0.0f;

    void apply_animation(const SkeletalAnimation& anim, float time, float weight);
};

// SkinnedMeshInstance3D — mesh que se deforma según un skeleton.
class SkinnedMeshInstance3D : public VoxMeshInstance3D {
public:
    ARX_CLASS(SkinnedMeshInstance3D, VoxMeshInstance3D);
public:

    void set_skeleton(VoxSkeleton3D* s) { skeleton_ = s; }
    VoxSkeleton3D* get_skeleton() const { return skeleton_; }

    void set_bone_attachments(const std::vector<std::pair<int, std::string>>& atts) {
        bone_attachments_ = atts;
    }

    void process(float delta) override;

    // Skin matrix palette (subida al shader como uniform array).
    const std::vector<Matrix4>& get_skin_palette() const { return skin_palette_; }

private:
    VoxSkeleton3D* skeleton_ = nullptr;
    std::vector<std::pair<int, std::string>> bone_attachments_;
    std::vector<Matrix4> skin_palette_;  // 4x4 por bone.
};

// IK (Inverse Kinematics) simple.
class IKChain {
public:
    void set_chain(const std::vector<int>& bone_indices) { chain_ = bone_indices; }
    void set_target(Vector3 target) { target_ = target; }
    void set_iterations(int n) { iterations_ = n; }
    void set_tolerance(float t) { tolerance_ = t; }

    // FABRIK solver.
    void solve(VoxSkeleton3D* skeleton);

private:
    std::vector<int> chain_;
    Vector3 target_{0, 0, 0};
    int     iterations_ = 10;
    float   tolerance_  = 0.01f;
};

} // namespace arx
