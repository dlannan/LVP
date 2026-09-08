#pragma once

#include "../../../common/state/include/state.h"
#include "../../../common/state/include/state_packet.h"
#include "../../../common/state/include/state_stream.h"

#include <cstdint>
#include <unordered_map>

using state_uid = std::uint64_t;

class stateControl : public state
{
public:
    stateControl();
    ~stateControl();

    stateControl(const stateControl&) = delete;
    stateControl& operator=(const stateControl&) = delete;

    bool Init(state_stream& input, state_stream& output) override;
    bool Begin() override;

    void PreUpdate() override;
    //void Update(int px, int py, int buttons) override;
    void Render() override;
    void PostUpdate() override;
   
    void Finish() override;

private:
    state_stream* m_input;
    state_stream* m_output;

    bool m_running;

    state_uid m_nextUid;
};