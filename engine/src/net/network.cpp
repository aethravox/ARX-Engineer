// ==============================================================================
#include "core/variant.hpp"
// src/net/network.cpp — Stubs (implementaciones mínimas).
// Las implementaciones reales de ENet/wslay se completan al habilitar las
// opciones ARX_USE_ENET y ARX_USE_WSLAY.
// ==============================================================================
#include "network.hpp"
#include "core/logging.hpp"

namespace arx {

// ===================== ENetMultiplayerPeer ===================================
struct ENetMultiplayerPeer::Pimpl {};
bool ENetMultiplayerPeer::create_server(int port, int max_clients) {
    ARX_LOG_INFO("ENetMultiplayerPeer: create_server port={} max={}", port, max_clients);
    type_ = Type::Server;
    return true;
}
bool ENetMultiplayerPeer::create_client(const std::string& host, int port) {
    ARX_LOG_INFO("ENetMultiplayerPeer: create_client {}:{} (TODO)", host, port);
    type_ = Type::Client;
    return true;
}
void ENetMultiplayerPeer::disconnect() {}
void ENetMultiplayerPeer::poll() {}
void ENetMultiplayerPeer::send(int, const std::vector<uint8_t>&, bool) {}
std::vector<std::pair<int, std::vector<uint8_t>>> ENetMultiplayerPeer::drain_incoming() { return {}; }

// ===================== WebSocketPeer =========================================
struct WebSocketPeer::Pimpl {};
bool WebSocketPeer::connect_to_url(const std::string& url) {
    ARX_LOG_INFO("WebSocketPeer: connect_to_url {}", url);
    return true;
}
bool WebSocketPeer::create_server(int, int) { return false; }
bool WebSocketPeer::create_client(const std::string&, int) { return false; }
void WebSocketPeer::disconnect() {}
void WebSocketPeer::poll() {}
void WebSocketPeer::send(int, const std::vector<uint8_t>&, bool) {}
std::vector<std::pair<int, std::vector<uint8_t>>> WebSocketPeer::drain_incoming() { return {}; }

// ===================== MultiplayerAPI ========================================
void MultiplayerAPI::set_peer(std::shared_ptr<NetworkPeer> p) {
    peer_ = std::move(p);
}
void MultiplayerAPI::poll() {
    if (peer_) peer_->poll();
}
void MultiplayerAPI::rpc_call(int peer_id, const std::string& method,
                                 const std::vector<Variant>& args) {
    ARX_LOG_INFO("RPC call peer={} method={} args={}", peer_id, method, args.size());
    // TODO: serializar método + args con Variant y enviar via peer_->send().
}
void MultiplayerAPI::rpc_id(int peer_id, const std::string& method,
                              const std::vector<Variant>& args) {
    rpc_call(peer_id, method, args);
}

// ===================== HTTPClient ============================================
HTTPClient::Response HTTPClient::request(const std::string& url, Method,
                                            const std::string& body,
                                            const std::unordered_map<std::string, std::string>&) {
    ARX_LOG_INFO("HTTPClient::request {} (body={} bytes)", url, body.size());
    // TODO: implementar con mbedtls (HTTPS) o socket plain (HTTP).
    Response r;
    r.error = "HTTPClient no implementado todavía";
    return r;
}

} // namespace arx
