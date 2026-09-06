#pragma once

#include "../../common/module/include/module.h"
#include "../../common/module/include/module_packet.h"
#include "../../common/module/include/module_stream.h"

#include <cstdint>
#include <unordered_map>

using module_uid = std::uint64_t;

class atlas
{
public:
    atlas();
    ~atlas();

    atlas(const atlas&) = delete;
    atlas& operator=(const atlas&) = delete;

    bool Init(module_stream& input, module_stream& output);
    bool Begin();

    void PreUpdate();
    int Update();
    void PostUpdate();

    void Finish();

private:
    module_stream* m_input;
    module_stream* m_output;

    bool m_running;

    module_uid m_nextUid;

    std::unordered_map<module_uid, module_envelope> m_modules;
};