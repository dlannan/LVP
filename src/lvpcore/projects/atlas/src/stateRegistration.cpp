#include "stateRegistration.h"

#include <string>
#include <iostream>

namespace
{
    constexpr const char* RegistrationTarget = "atlas";
    constexpr const char* RegistrationCommand = "register";
}

stateRegistration::stateRegistration()
    : m_nextUid(1)
{

}
stateRegistration::~stateRegistration()
{

}

bool stateRegistration::Init(state_stream& input, state_stream& output)  
{
    m_input = &input;
    m_output = &output;

    return true;
}

bool stateRegistration::Begin()
{
    std::cout << "stateRegistration: Begin\n";

    if (m_input == nullptr || m_output == nullptr)
    {
        return false;
    }

    m_running = true;

    return true;
}

void stateRegistration::PreUpdate()
{

}

void stateRegistration::Update(int px, int py, int buttons)
{
    if (!m_running || m_input == nullptr)
    {
        return;
    }

    int processed = 0;

    state_packet packet;

    while (m_input->read(packet))
    {
        ++processed;

        std::cout
            << "atlas: received packet"
            << " uid=" << packet.envelope.uid
            << " owner=" << packet.envelope.owner
            << " target=" << packet.envelope.target
            << " scope=" << packet.envelope.memoryScope
            << '\n';

        bool isRegistration = false;

        for (const auto& metadata : packet.metadata)
        {
            if (metadata.key == "command" &&
                metadata.value == RegistrationCommand)
            {
                isRegistration = true;
                break;
            }
        }

        if (!isRegistration)
        {
            std::cout << "atlas: ignoring packet\n";
            continue;
        }

        if (packet.envelope.target != RegistrationTarget)
        {
            std::cout
                << "atlas: registration packet has incorrect target\n";

            continue;
        }

        const state_uid uid = m_nextUid++;

        m_states.emplace(
            uid,
            packet.envelope);

        std::cout
            << "atlas: registration request from "
            << packet.envelope.owner
            << " assigned uid="
            << uid
            << '\n';

        state_packet response;

        response.envelope.uid = uid;
        response.envelope.owner = "atlas";
        response.envelope.target = packet.envelope.owner;
        response.envelope.memoryScope = packet.envelope.memoryScope;

        response.metadata.push_back(
            {
                "command",
                "registered"
            });

        if (!m_output->write(response))
        {
            std::cout
                << "atlas: failed to queue registration response\n";
        }
    }

    m_processed = processed;
}

void stateRegistration::Render()
{

}

void stateRegistration::PostUpdate()
{

}

void stateRegistration::Finish()
{
    if (!m_running)
    {
        return;
    }

    std::cout << "atlas: Finish\n";

    m_running = false;
}