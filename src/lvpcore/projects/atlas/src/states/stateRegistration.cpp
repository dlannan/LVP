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
    bool do_process = true;

    state_packet packet;

    if(m_input->check(packet))
    {
        ++processed;

        std::cout << "reg Update\n" ;
        std::cout
            << "stateRegistration: received packet"
            << " uid=" << packet.envelope.uid
            << " owner=" << packet.envelope.owner
            << " target=" << packet.envelope.target
            << " scope=" << packet.envelope.memoryScope
            << '\n';

        bool isRegistration = false;

        for (const auto& metadata : packet.metadata)
        {
            std::cout << metadata.key << ":" << metadata.value << "\n";
            if (metadata.key == "command" && metadata.value == RegistrationCommand)
            {
                isRegistration = true;
                break;
            }
        }

        if (!isRegistration)
        {
            std::cout << "stateRegistration: ignoring packet\n";
            do_process = false;
        }

        if (packet.envelope.target != RegistrationTarget)
        {
            std::cout << "stateRegistration: registration packet has incorrect target\n";
            do_process = false;
        }

        if(do_process) {

            m_input->read(packet);
            const state_uid uid = m_nextUid++;
            m_states.emplace(uid, packet.envelope);

            std::cout
                << "stateRegistration: registration request from "
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
                std::cout << "stateRegistration: failed to queue registration response\n";
            }
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

    std::cout << "stateRegistration: Finish\n";

    m_running = false;
}