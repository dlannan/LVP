#pragma once

#include "../../../common/state/include/state.h"
#include "../../../common/state/include/state_packet.h"
#include "../../../common/state/include/state_stream.h"
#include "../../../common/state/include/state_manager.h"

#include <cstdint>
#include <unordered_map>

using state_uid = std::uint64_t;

class stateTestForward : public state
{
public:
    stateTestForward(std::string name, std::string target, state_manager *smanager);
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

    // Number of forwarded successes
    int m_processed = 0;
    std::string m_target;
    std::uint64_t m_ipuid = 0;

private:
    void TestForward();

    state_manager *m_smanager;

    double  m_lasttime = 0.0;
    double  m_current = 0.0;

    state_stream* m_input;
    state_stream* m_output;

    std::string m_name;

    bool m_running;
};