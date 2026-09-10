#include "stateForwarding.h"

#include <string>
#include <iostream>

namespace
{
    constexpr const char* ForwardingCommand = "forward";
}


stateForwarding::stateForwarding() 
{

}
stateForwarding::~stateForwarding()
{

}

bool stateForwarding::Init(state_stream& input, state_stream& output)  
{
    m_input = &input;
    m_output = &output;
    return true;
}

bool stateForwarding::Begin()
{
    std::cout << "stateForwarding: Begin\n";
    if (m_input == nullptr || m_output == nullptr)
    {
        return false;
    }

    m_running = true;
    return true;
}

void stateForwarding::PreUpdate()
{

}

// Forwarding is super simple:
//     1. Lookup metadata - is forwarding req?
//     2. Forwarding, get target - is it registered?
//     3. If registereed, send packet to target
//
void stateForwarding::Update(int px, int py, int buttons)
{
    if (!m_running || m_input == nullptr)
    {
        return;
    }

    int processed = 0;

    state_packet packet;

    if(m_input->check(packet))
    {
        ++processed;

        std::cout
            << "stateForwarding: received packet"
            << " uid=" << packet.envelope.uid
            << " owner=" << packet.envelope.owner
            << " target=" << packet.envelope.target
            << " scope=" << packet.envelope.memoryScope
            << '\n';

        bool isForwarding = false;
        for (const auto& metadata : packet.metadata)
        {
            if (metadata.key == "command" && metadata.value == ForwardingCommand)
            {
                isForwarding = true;
                break;
            }
        }

        if (!isForwarding)
        {
            // std::cout << "stateForwarding: ignoring packet\n";
            return;
        }

        m_input->read(packet);
        std::cout
            << "stateForwarding: forward request from "
            << packet.envelope.owner
            << " assigned uid="
            << packet.envelope.uid
            << " to "
            << packet.envelope.target
            << '\n';        

        state_packet response;

        response.envelope.uid           = packet.envelope.uid;
        response.envelope.owner         = packet.envelope.owner;
        response.envelope.target        = packet.envelope.target;
        response.envelope.memoryScope   = packet.envelope.memoryScope;

        response.metadata.push_back(
            {
                "command",
                "forwarded"
            });

        if (!m_output->write(response))
        {
            std::cout << "stateForwarding: failed to queue forwarded response\n";
        }
    }
}

void stateForwarding::Render()
{
}

void stateForwarding::PostUpdate()
{

}

void stateForwarding::Finish()
{

}