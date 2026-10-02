// ==============================================================================
// src/network/network.hpp — Networking high-level sobre ENet.
// ==============================================================================
#pragma once

#include "core/types.hpp"
#include "core/object.hpp"

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <unordered_map>
#include <cstdint>

namespace arx {

// NetworkPeer — conexión P2P sobre ENet.
class NetworkPeer : public Object {
public:
    ARX_CLASS(NetworkPeer, Object);
public:

    enum class Mode { Server, Client, Mesh };

    bool create_server(int port, int max_clients = 32,
                        const std::string& bind_address = "*");
    bool create_client(const std::string& address, int port,
                        int client_count = 1, int channel_count = 2);
    void close();
    void poll();   // Procesar eventos encolados.
    void set_target_peer(int id) { target_peer_ = id; }

    // RPC: invoca un método en el/los peer(s) remoto(s).
    void rpc(int peer_id, const std::string& method,
              const std::vector<Variant>& args = {});
    void rpc_id(int peer_id, const std::string& method,
                const std::vector<Variant>& args) {
        rpc(peer_id, method, args);
    }
    void rpc_unreliable(int peer_id, const std::string& method,
                         const std::vector<Variant>& args);

    int  get_unique_id() const { return unique_id_; }
    Mode get_mode() const { return mode_; }
    bool is_server() const { return mode_ == Mode::Server; }
    bool is_client() const { return mode_ == Mode::Client; }

    std::vector<int> get_peer_ids() const;

    // Callbacks.
    void on_peer_connected(std::function<void(int)> cb)    { on_peer_conn_ = std::move(cb); }
    void on_peer_disconnected(std::function<void(int)> cb) { on_peer_disc_ = std::move(cb); }
    void on_packet_received(std::function<void(int, const std::vector<uint8_t>&)> cb) {
        on_packet_ = std::move(cb);
    }

private:
    Mode mode_ = Mode::Server;
    int  unique_id_ = 1;
    int  target_peer_ = 0;
    std::function<void(int)>                                  on_peer_conn_;
    std::function<void(int)>                                  on_peer_disc_;
    std::function<void(int, const std::vector<uint8_t>&)>     on_packet_;

    struct ENetHost*   host_  = nullptr;
    struct ENetPeer*   server_ = nullptr;  // Si es client, el peer del server.
    std::unordered_map<int, struct ENetPeer*> peers_;
    int next_peer_id_ = 2;
};

// MultiplayerAPI — singleton que enrta RPCs hacia el SceneTree.
class MultiplayerAPI {
public:
    static MultiplayerAPI& instance();

    void set_peer(std::shared_ptr<NetworkPeer> peer);
    std::shared_ptr<NetworkPeer> get_peer() const { return peer_; }

    void poll();   // Llamar cada frame.
    void process_rpc(int sender_peer, const std::string& method,
                      const std::vector<Variant>& args);

private:
    MultiplayerAPI() = default;
    std::shared_ptr<NetworkPeer> peer_;
};

} // namespace arx
