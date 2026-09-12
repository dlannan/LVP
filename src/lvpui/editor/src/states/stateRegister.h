#pragma once

#include "../../common/state/include/state.h"
#include "../../common/state/include/state_packet.h"
#include "../../common/state/include/state_stream.h"
#include "../../common/state/include/state_manager.h"

#include <cstdint>
#include <unordered_map>

using state_uid = std::uint64_t;

class stateRegister : public state
{
public:
    stateRegister(std::string name, state_manager *smanager);
    ~stateRegister();

    stateRegister(const stateRegister&) = delete;
    stateRegister& operator=(const stateRegister&) = delete;

    bool Init(state_stream& input, state_stream& output) override;
    bool Begin() override;

    void PreUpdate() override;
    void Update(int px, int py, int buttons) override;
    void Render() override;
    void PostUpdate() override;

    void Finish() override;

    // Just used for general state sharing
    int m_processed = 0;
    bool m_registered = false;

private:
    state_manager   *m_smanager;

    state_stream*   m_input;
    state_stream*   m_output;

    std::string     m_name;

    bool            m_running;
};