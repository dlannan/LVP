#pragma once

#include "../../common/state/include/state.h"
#include "../../common/state/include/state_packet.h"
#include "../../common/state/include/state_stream.h"

#include <cstdint>

class dummy: state
{
public:
    dummy(const char *name);
    ~dummy();

    dummy(const dummy&) = delete;
    dummy& operator=(const dummy&) = delete;

    bool Init(state_stream& input, state_stream& output);
    bool Begin();

    void PreUpdate();
    int Update();
    void PostUpdate();

    void Finish();

private:
    state_stream* m_input;
    state_stream* m_output;

    std::uint64_t m_uid;
    std::string m_name;

    bool m_running;
};