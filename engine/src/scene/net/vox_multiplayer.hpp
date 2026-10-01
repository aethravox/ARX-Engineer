// ==============================================================================
// src/scene/net/vox_multiplayer.hpp — Voxes de networking.
// ==============================================================================
#pragma once

#include "scene/vox.hpp"
#include "net/network.hpp"

#include <string>
#include <vector>
#include <memory>

namespace arx {

class VoxMultiplayerSpawner : public Vox {
public:
    ARX_CLASS(VoxMultiplayerSpawner, Vox);
public:
    void set_peer(std::shared_ptr<NetworkPeer> p) { peer_ = p; }
    std::shared_ptr<NetworkPeer> get_peer() const { return peer_; }
    void set_spawn_path(const std::string& p) { spawn_path_ = p; }
    const std::string& get_spawn_path() const { return spawn_path_; }
    void spawn(Vox* vox) {}
    void despawn(Vox* vox) {}
    void process(float delta) {
        if (!peer_) return;
        peer_->poll();
        auto incoming = peer_->drain_incoming();
        (void)incoming;
    }
private:
    std::shared_ptr<NetworkPeer> peer_;
    std::string spawn_path_ = ".";
};

class VoxMultiplayerSynchronizer : public Vox {
public:
    ARX_CLASS(VoxMultiplayerSynchronizer, Vox);
public:
    void set_peer(std::shared_ptr<NetworkPeer> p) { peer_ = p; }
    void set_position_sync(bool s) { sync_pos_ = s; }
    void set_rotation_sync(bool s) { sync_rot_ = s; }
    void set_scale_sync(bool s) { sync_scl_ = s; }
    bool get_position_sync() const { return sync_pos_; }
    bool get_rotation_sync() const { return sync_rot_; }
    bool get_scale_sync() const { return sync_scl_; }
    void process(float delta) { (void)delta; }
private:
    std::shared_ptr<NetworkPeer> peer_;
    bool sync_pos_ = true;
    bool sync_rot_ = false;
    bool sync_scl_ = false;
};

} // namespace arx
