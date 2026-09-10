
#include "../../common/state/include/state.h"
#include "../../common/packet_interface/include/packet_interface.h"

#include "../../common/state/include/state.h"
#include "../../common/state/include/state_packet.h"
#include "../../common/state/include/state_stream.h"
#include "../../common/state/include/state_manager.h"

#include "./states/stateRegister.h"
#include "./states/stateTestForward.h"

#include <cstdint>
#include <iostream>
#include <string>

#include <windows.h>

static state_manager   m_smanager;

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        std::cerr << "usage: dummystate <name> <port> <target>\n";
        return 1;
    }

    std::string nameValue = std::string(argv[1]);
    const int portValue = std::stoi(argv[2]);
    std::string targetValue = std::string(argv[3]);

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

    stateRegister m_register( nameValue, &m_smanager );
    stateTestForward m_testforward( nameValue, targetValue, &m_smanager );

    m_register.Init(input, output);
    m_testforward.Init(input, output);

    m_smanager.Init();
    m_smanager.CreateState("dummy_register", &m_register);
    m_smanager.CreateState("dummy_testforward", &m_testforward);

    std::cout << "dummy: Init\n";

    m_smanager.ChangeState("dummy_register");
    
    std::cout <<  nameValue << ": running...\n";
    bool running = true;
    uint64_t start = GetTickCount64();

    while (m_smanager.Run())
    {
        uint64_t tm = GetTickCount64();
        m_smanager.SetDelta( ((double)tm - (double)start) * 0.001 );
        start = tm;

        const int packetResult = pi.Update();

        //m_smanager.PreUpdate();

        m_smanager.Update(0, 0, 0);

        //m_smanager.PostUpdate();
    }

    // m_smanager.Finish();
    pi.Finish();

    return 0;
}