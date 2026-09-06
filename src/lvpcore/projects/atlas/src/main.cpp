#include "atlas.h"

#include "../../common/state/include/state.h"
#include "../../common/packet_interface/include/packet_interface.h"

#include <cstdint>
#include <iostream>

int main()
{
    constexpr std::uint16_t AtlasPort = 42000;

    std::cout << "atlas starting...\n";

    packet_interface_config config;

    config.port = AtlasPort;
    config.atlas = "atlas";
    config.role = packet_interface_role::atlas;

    packet_interface pi(config);


    if (!pi.Init())
    {
        std::cerr << "atlas: packet_interface initialisation failed\n";
        return 1;
    }

    state_stream& input = pi.readStream();
    state_stream& output = pi.writeStream();

    atlas master;

    if (!master.Init(input, output))
    {
        std::cerr << "atlas: Init failed\n";
        pi.Finish();
        return 1;
    }

    if (!master.Begin())
    {
        std::cerr << "atlas: Begin failed\n";
        master.Finish();
        pi.Finish();
        return 1;
    }

    bool running = true;

    std::cout << "atlas running...\n";

    while (running)
    {
        const int packetStatus = pi.Update();

        master.PreUpdate();

        master.Update();

        master.PostUpdate();
    }

    master.Finish();
    pi.Finish();

    return 0;
}