#pragma once

#include "../../state/include/state_stream.h"

#include <condition_variable>
#include <mutex>

#include <cstdint>
#include <string>

enum class packet_interface_atlas_domain
{
    local,
    dns,
    ip
};

enum class packet_interface_role
{
    state,
    atlas
};

struct packet_interface_config
{
    std::uint16_t port = 0;
    std::string atlas;
    packet_interface_atlas_domain domain = packet_interface_atlas_domain::local;
    packet_interface_role role = packet_interface_role::state;
};

class packet_interface
{
    public:
    explicit packet_interface(
    const packet_interface_config& config);

    ~packet_interface();

    packet_interface(const packet_interface&) = delete;
    packet_interface& operator=(const packet_interface&) = delete;

    bool Init();

    int Update();

    void Finish();

    void Wait();

    state_stream& readStream();
    state_stream& writeStream();


private:
    class stream;
    struct transport;

    packet_interface_config m_config;

    stream* m_readStream;
    stream* m_writeStream;

    transport* m_transport;

    mutable std::mutex m_mutex;
    std::condition_variable m_cv;
};