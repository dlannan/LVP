#include "atlas.h"

#include "../../common/module/include/module.h"
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

    module_stream& input = pi.readStream();
    module_stream& output = pi.writeStream();

    atlas module;

    if (!module.Init(input, output))
    {
        std::cerr << "atlas: Init failed\n";
        pi.Finish();
        return 1;
    }

    if (!module.Begin())
    {
        std::cerr << "atlas: Begin failed\n";
        module.Finish();
        pi.Finish();
        return 1;
    }

    std::cout << "atlas running...\n";

    bool running = true;

    while (running)
    {
        const int packetStatus = pi.Update();

        module.PreUpdate();

        const int moduleStatus = module.Update();

        module.PostUpdate();

        /*
         * Status handling will be defined later.
         *
         * For now, keep the loop alive and simply
         * allow both subsystems to advance.
         */
        (void)packetStatus;
        (void)moduleStatus;
    }

    module.Finish();
    pi.Finish();

    return 0;
}