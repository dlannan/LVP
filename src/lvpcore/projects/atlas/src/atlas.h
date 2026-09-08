#pragma once

#include "../../common/state/include/state.h"
#include "../../common/state/include/state_packet.h"
#include "../../common/state/include/state_stream.h"
#include "../../common/state/include/state_manager.h"

#include <cstdint>
#include <unordered_map>

#include "states/stateRegistration.h"
#include "states/stateForwarding.h"
#include "states/stateControl.h"

using state_uid = std::uint64_t;

class atlas : state
{
public:
    atlas();
    ~atlas();

    atlas(const atlas&) = delete;
    atlas& operator=(const atlas&) = delete;

    bool Init(state_stream& input, state_stream& output) override;
    bool Begin() override;

    void PreUpdate();
    void Update();
    void PostUpdate();

    void Finish() override;

private:
    state_stream* m_input;
    state_stream* m_output;

    bool m_running;

    stateRegistration m_registration;
    stateForwarding m_forwarding;
    stateControl m_control;

    state_manager   m_smanager;
    state_uid m_nextUid;

};