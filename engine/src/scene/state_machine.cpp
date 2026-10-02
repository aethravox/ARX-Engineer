// ==============================================================================
// src/scene/state_machine.cpp
// ==============================================================================
#include "state_machine.hpp"
#include "core/logging.hpp"

namespace arx {

void StateMachine::add_state(State s) {
    states_[s.name] = std::move(s);
}

void StateMachine::remove_state(const std::string& name) {
    states_.erase(name);
    if (current_ == name) current_.clear();
}

bool StateMachine::has_state(const std::string& name) const {
    return states_.find(name) != states_.end();
}

void StateMachine::set_initial_state(const std::string& name) {
    initial_ = name;
}

void StateMachine::transition_to(const std::string& state_name) {
    auto it = states_.find(state_name);
    if (it == states_.end()) {
        ARX_LOG_WARN("StateMachine: estado '{}' no existe", state_name);
        return;
    }
    if (!current_.empty()) {
        auto cit = states_.find(current_);
        if (cit != states_.end() && cit->second.on_exit) cit->second.on_exit();
    }
    current_ = state_name;
    if (it->second.on_enter) it->second.on_enter();
}

void StateMachine::event(const std::string& event_name) {
    if (current_.empty()) return;
    auto it = states_.find(current_);
    if (it == states_.end()) return;
    auto tit = it->second.transitions.find(event_name);
    if (tit != it->second.transitions.end()) {
        transition_to(tit->second);
    }
}

std::vector<std::string> StateMachine::get_states() const {
    std::vector<std::string> out;
    out.reserve(states_.size());
    for (const auto& [n, _] : states_) out.push_back(n);
    return out;
}

void StateMachine::process(float delta) {
    if (!started_) {
        if (!initial_.empty()) transition_to(initial_);
        started_ = true;
        return;
    }
    auto it = states_.find(current_);
    if (it != states_.end() && it->second.on_process) it->second.on_process(delta);
}

void StateMachine::_physics_process(float delta) {
    auto it = states_.find(current_);
    if (it != states_.end() && it->second.on_physics_process)
        it->second.on_physics_process(delta);
}

} // namespace arx
