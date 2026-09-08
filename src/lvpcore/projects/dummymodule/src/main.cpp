#include "state.h"

#include "../../common/packet_interface/include/packet_interface.h"

#include <cstdint>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "usage: dummystate <name> <port>\n";
        return 1;
    }

    std::string nameValue = std::string(argv[1]);
    const int portValue = std::stoi(argv[2]);

    if (portValue < 1 || portValue > 65535)
    {
        std::cerr << "dummystate: invalid port\n";
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
    config.role = packet_interface_role::state;

    packet_interface pi(config);

    if (!pi.Init())
    {
        std::cerr << nameValue << ": packet_interface initialisation failed\n";
        return 1;
    }

    state_stream& input = pi.readStream();
    state_stream& output = pi.writeStream();

    dummy stateRegister(nameValue.c_str());

    if (!stateRegister.Init(input, output))
    {
        std::cerr << nameValue << ": Init failed\n";

        pi.Finish();

        return 1;
    }

    if (!stateRegister.Begin())
    {
        std::cerr << nameValue << ": Begin failed\n";

        stateRegister.Finish();
        pi.Finish();

        return 1;
    }

    std::cout <<  nameValue << ": running...\n";
    bool running = true;

    while (running)
    {
        const int packetResult = pi.Update();

        stateRegister.PreUpdate();

        const int stateRegisterResult = stateRegister.Update();

        stateRegister.PostUpdate();

    }

    stateRegister.Finish();
    pi.Finish();

    return 0;
}