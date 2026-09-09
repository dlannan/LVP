#pragma once

#include "../../../common/state/include/state.h"
#include "../../../common/state/include/state_packet.h"
#include "../../../common/state/include/state_stream.h"

#include <cstdint>
#include <unordered_map>

using state_uid = std::uint64_t;

class stateTestForward : public state
{
public:
    stateTestForward();
    ~stateTestForward();

    stateTestForward(const stateTestForward&) = delete;
    stateTestForward& operator=(const stateTestForward&) = delete;

    bool Init(state_stream& input, state_stream& output) override;
    bool Begin() override;

    void PreUpdate() override;
    void Update(int px, int py, int buttons) override;
    void Render() override;
    void PostUpdate() override;

    void Finish() override;

private:
    state_stream* m_input;
    state_stream* m_output;

    std::uint64_t m_uid;
    std::string m_name;

    bool m_running;
};