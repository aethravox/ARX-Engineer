// ==============================================================================
// src/scene/state_machine.hpp — State machine node.
// Máquina de estados finita para lógica de gameplay.
// ==============================================================================
#pragma once

#include "scene/vox.hpp"
#include "core/types.hpp"

#include <string>
#include <unordered_map>
#include <functional>
#include <vector>

namespace arx {

class StateMachine : public Vox {
public:
    ARX_CLASS(StateMachine, Vox);
public:

    struct State {
        std::string name;
        std::function<void()> on_enter;
        std::function<void(float)> on_process;
        std::function<void(float)> on_physics_process;
        std::function<void()> on_exit;
        std::unordered_map<std::string, std::string> transitions;  // event → target
    };

    void add_state(State s);
    void remove_state(const std::string& name);
    bool has_state(const std::string& name) const;

    void set_initial_state(const std::string& name);
    void transition_to(const std::string& state_name);
    void event(const std::string& event_name);

    const std::string& get_current_state() const { return current_; }
    std::vector<std::string> get_states() const;

    void process(float delta) override;
    void _physics_process(float delta) override;

private:
    std::unordered_map<std::string, State> states_;
    std::string current_;
    std::string initial_;
    bool started_ = false;
};

} // namespace arx
