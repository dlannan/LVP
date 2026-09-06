#include "module.h"

#include "../../common/packet_interface/include/packet_interface.h"

#include <cstdint>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "usage: dummymodule <port>\n";
        return 1;
    }

    const int portValue = std::stoi(argv[1]);

    if (portValue < 1 || portValue > 65535)
    {
        std::cerr << "dummymodule: invalid port\n";
        return 1;
    }

    const std::uint16_t DummyPort = static_cast<std::uint16_t>(portValue);

    std::cout
        << "dummy starting on port "
        << DummyPort
        << "...\n";

    packet_interface_config config;

    config.port = DummyPort;
    config.atlas = "atlas";
    config.role = packet_interface_role::module;

    packet_interface pi(config);

    if (!pi.Init())
    {
        std::cerr << "dummy: packet_interface initialisation failed\n";
        return 1;
    }

    module_stream& input = pi.readStream();
    module_stream& output = pi.writeStream();

    dummy module;

    if (!module.Init(input, output))
    {
        std::cerr << "dummy: Init failed\n";

        pi.Finish();

        return 1;
    }

    if (!module.Begin())
    {
        std::cerr << "dummy: Begin failed\n";

        module.Finish();
        pi.Finish();

        return 1;
    }

    std::cout << "dummy running...\n";
    bool running = true;

    while (running)
    {
        const int packetResult = pi.Update();

        module.PreUpdate();

        const int moduleResult = module.Update();

        module.PostUpdate();

        (void)packetResult;
        (void)moduleResult;
    }

    module.Finish();
    pi.Finish();

    return 0;
}