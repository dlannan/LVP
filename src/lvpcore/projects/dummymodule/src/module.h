#pragma once

#include "../../common/module/include/module.h"
#include "../../common/module/include/module_packet.h"
#include "../../common/module/include/module_stream.h"

#include <cstdint>

class dummy
{
public:
    dummy();
    ~dummy();

    dummy(const dummy&) = delete;
    dummy& operator=(const dummy&) = delete;

    bool Init(module_stream& input, module_stream& output);
    bool Begin();

    void PreUpdate();
    int Update();
    void PostUpdate();

    void Finish();

private:
    module_stream* m_input;
    module_stream* m_output;

    std::uint64_t m_uid;

    bool m_running;
};