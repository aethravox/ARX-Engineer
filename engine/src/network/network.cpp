// ==============================================================================
#include "core/variant.hpp"
// src/network/network.cpp — Stub simplificado de ENet.
// En una implementación real, envolvería enet_host_create/enet_host_service/etc.
// Aquí se mantiene el código compilable pero funcional solo en el sentido de
// "no crashea si no hay red".
// ==============================================================================
#include "network.hpp"
#include "core/logging.hpp"

#include <cstring>

// Forward declarations de ENet (real cuando se incluye <enet/enet.h>).
namespace arx {

bool NetworkPeer::create_server(int port, int max_clients,
                                  const std::string& bind_address) {
    (void)port; (void)max_clients; (void)bind_address;
    mode_ = Mode::Server;
    ARX_LOG_INFO("NetworkPeer: servidor creado en puerto {} (max {})", port, max_clients);
    // TODO: enet_host_create(...)
    return true;
}

bool NetworkPeer::create_client(const std::string& address, int port,
                                  int client_count, int channel_count) {
    (void)address; (void)port; (void)client_count; (void)channel_count;
    mode_ = Mode::Client;
    ARX_LOG_INFO("NetworkPeer: cliente conectando a {}:{}", address, port);
    return true;
}

void NetworkPeer::close() {
    ARX_LOG_INFO("NetworkPeer: cerrado");
}

void NetworkPeer::poll() {
    // En una impl real: enet_host_service(host_, &event, 0) y despachar.
}

void NetworkPeer::rpc(int peer_id, const std::string& method,
                       const std::vector<Variant>& args) {
    (void)peer_id; (void)method; (void)args;
    // Serializar method + args y enviar por el canal 0 (reliable).
}

void NetworkPeer::rpc_unreliable(int peer_id, const std::string& method,
                                   const std::vector<Variant>& args) {
    (void)peer_id; (void)method; (void)args;
    // Igual que rpc pero por canal 1 (unreliable).
}

std::vector<int> NetworkPeer::get_peer_ids() const {
    std::vector<int> ids;
    for (const auto& [id, _] : peers_) ids.push_back(id);
    return ids;
}

// ===================== MultiplayerAPI ========================================
MultiplayerAPI& MultiplayerAPI::instance() {
    static MultiplayerAPI m;
    return m;
}

void MultiplayerAPI::set_peer(std::shared_ptr<NetworkPeer> peer) {
    peer_ = std::move(peer);
}

void MultiplayerAPI::poll() {
    if (peer_) peer_->poll();
}

void MultiplayerAPI::process_rpc(int sender_peer, const std::string& method,
                                   const std::vector<Variant>& args) {
    (void)sender_peer;
    ARX_LOG_DEBUG("RPC recv: {} ({} args)", method, args.size());
    // En una impl completa, esto buscaría un Vox por path y llamaría
    // call_method(sid(method), args).
}

} // namespace arx
