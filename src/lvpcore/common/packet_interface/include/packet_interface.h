#pragma once

#include "../../module/include/module_stream.h"

#include <cstdint>
#include <string>

enum class packet_interface_role
{
    module,
    atlas
};

struct packet_interface_config
{
    std::uint16_t port = 0;
    std::string atlas;
    packet_interface_role role = packet_interface_role::module;
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

    module_stream& readStream();
    module_stream& writeStream();


private:
    class stream;
    struct transport;

    packet_interface_config m_config;

    stream* m_readStream;
    stream* m_writeStream;

    transport* m_transport;
};