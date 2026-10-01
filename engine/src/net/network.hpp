// ==============================================================================
// src/net/network.hpp — Networking multi-peer (estilo Godot MultiplayerAPI).
// Wrappers sobre ENet + WebSocket (wslay).
// ==============================================================================
#pragma once

#include "core/types.hpp"
#include "core/object.hpp"

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <cstdint>

namespace arx {

// Peer: conexión a un cliente/servidor remoto.
class NetworkPeer : public Object {
public:
    ARX_CLASS(NetworkPeer, Object);
public:
    enum class Type { None, Server, Client, Mesh };

    virtual bool create_server(int port, int max_clients = 32) = 0;
    virtual bool create_client(const std::string& host, int port) = 0;
    virtual void disconnect() = 0;
    virtual void poll() = 0;
    virtual void set_target_peer(int id) { target_peer_ = id; }
    int  get_target_peer() const { return target_peer_; }

    // RPC: enviar datos a un peer.
    virtual void send(int peer_id, const std::vector<uint8_t>& data, bool reliable = true) = 0;
    virtual std::vector<std::pair<int, std::vector<uint8_t>>> drain_incoming() = 0;

    Type get_type() const { return type_; }
    bool is_server() const { return type_ == Type::Server; }
    bool is_client() const { return type_ == Type::Client; }
    int  get_unique_id() const { return unique_id_; }

    // Signals: peer_connected(id), peer_disconnected(id), packet_received(id, data)

protected:
    Type type_       = Type::None;
    int  unique_id_  = 1;
    int  target_peer_ = 0;
};

// ENetMultiplayerPeer: implementación con ENet (UDP confiable).
class ENetMultiplayerPeer : public NetworkPeer {
public:
    ARX_CLASS(ENetMultiplayerPeer, NetworkPeer);
public:

    bool create_server(int port, int max_clients = 32) override;
    bool create_client(const std::string& host, int port) override;
    void disconnect() override;
    void poll() override;
    void send(int peer_id, const std::vector<uint8_t>& data, bool reliable = true) override;
    std::vector<std::pair<int, std::vector<uint8_t>>> drain_incoming() override;

private:
    struct Pimpl;
    std::unique_ptr<Pimpl> p_;
};

// WebSocketPeer: cliente WS basado en wslay.
class WebSocketPeer : public NetworkPeer {
public:
    ARX_CLASS(WebSocketPeer, NetworkPeer);
public:

    bool connect_to_url(const std::string& url);
    bool create_server(int port, int max_clients = 32) override;
    bool create_client(const std::string& host, int port) override;
    void disconnect() override;
    void poll() override;
    void send(int peer_id, const std::vector<uint8_t>& data, bool reliable = true) override;
    std::vector<std::pair<int, std::vector<uint8_t>>> drain_incoming() override;

private:
    struct Pimpl;
    std::unique_ptr<Pimpl> p_;
};

// MultiplayerAPI: alto nivel. RPC, replicación, authority.
class MultiplayerAPI : public Object {
public:
    ARX_CLASS(MultiplayerAPI, Object);
public:

    void set_peer(std::shared_ptr<NetworkPeer> p);
    std::shared_ptr<NetworkPeer> get_peer() const { return peer_; }

    void poll();
    bool has_multiplayer_peer() const { return peer_ != nullptr; }
    int  get_unique_id() const { return peer_ ? peer_->get_unique_id() : 1; }
    bool is_server() const { return peer_ && peer_->is_server(); }

    // RPC: invocar método en peers remotos.
    void rpc_call(int peer_id, const std::string& method,
                  const std::vector<Variant>& args);
    void rpc_id(int peer_id, const std::string& method,
                const std::vector<Variant>& args);

    // Para registrar métodos RPC en una clase:
    //   ARX_RPC(my_method, "arg1,arg2")
    // → genera un wrapper que serializa los args y llama rpc_call.

    // Signals: peer_connected(id), peer_disconnected(id), rpc_message(method, args)

private:
    std::shared_ptr<NetworkPeer> peer_;
    std::unordered_map<int, std::string> peer_paths_;
};

// HTTPClient: cliente HTTP/1.1 + HTTPS (via mbedtls).
class HTTPClient : public Object {
public:
    ARX_CLASS(HTTPClient, Object);
public:

    enum class Method { Get, Post, Put, Delete, Head, Patch };

    struct Response {
        int         status_code = 0;
        std::string body;
        std::unordered_map<std::string, std::string> headers;
        bool        success = false;
        std::string error;
    };

    Response request(const std::string& url, Method method = Method::Get,
                      const std::string& body = "",
                      const std::unordered_map<std::string, std::string>& headers = {});
};

} // namespace arx
