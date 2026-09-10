#include "stateTestForward.h"

#include <string>
#include <iostream>

// TestForward
//  What it is: Its simply a process that can be used to test forwarding a packet from 
//              one named process to another through atlas.
//  Command params: <name> <port> <target> 
//       name - the name of this state to be used
//       port - the port this process will use when atlas registers it
//       target - the test target process that packets will be sent to every second
//
//  The result of running two test forward processes targeting each other should be a 1 sec 
//    packet loop between the two. 
//  Adding a third, then one process will recieve two forwarded packets per second and so on.

namespace
{
    constexpr const char* RegisterCommand = "register";
    constexpr const char* RegisterResponse = "registered";
    constexpr const char* ForwardCommand = "forward";
    constexpr const char* ForwardResponse = "forwarded";
}

stateTestForward::stateTestForward(std::string name, std::string target, state_manager *smanager)
    : m_input(nullptr)
    , m_output(nullptr)
    , m_running(false)
    , m_name(name)
    , m_target(target)
    , m_smanager(smanager)
{

}
stateTestForward::~stateTestForward()
{
    Finish();
}

bool stateTestForward::Init(state_stream& input, state_stream& output)  
{
    m_input = &input;
    m_output = &output;

    std::cout << m_name << ": Init\n";
    return true;
}

bool stateTestForward::Begin()
{
    if (m_input == nullptr || m_output == nullptr)
    {
        return false;
    }

    std::cout << m_name << ": Begin\n";
    m_running = true;
    m_lasttime = dt;
    return true;
}

void stateTestForward::TestForward()
{
    state_packet packet;

    m_uid = m_smanager->GetState("dummy_register")->getUid();
    packet.envelope.uid = m_uid;
    std::cout << "m_uid: " << m_uid << "\n";
    packet.envelope.owner = m_name.c_str();
    packet.envelope.target = m_target.c_str();
    packet.envelope.memoryScope = "local";
    packet.metadata.push_back({ "command", ForwardCommand });

    if (!m_output->write(packet))
    {
        std::cout << m_name << ": failed to queue forward\n";
        return;
    }

    std::cout << m_name << ": forward queued\n";
}

void stateTestForward::PreUpdate()
{

}

void stateTestForward::Update(int px, int py, int buttons)
{
    if (!m_running || m_input == nullptr)
        return;

    int processed = 0;

    m_current += dt;
    if(m_current > m_lasttime + 1.0) {
        TestForward();
        m_lasttime = m_current;
    }

    state_packet packet;

    if(m_input->check(packet))
    {
        ++processed;

        bool isForwarded = false;

        for (const auto& metadata : packet.metadata)
        {
            std::cout << metadata.key << ":" << metadata.value << "\n";
            if (metadata.key == "command" && metadata.value == ForwardResponse)
            {
                isForwarded = true;
                break;
            }
        }

        if (!isForwarded)
        {
            //std::cout << m_name << ": ignoring packet\n";
            m_input->read(packet);
            return;
        }

        if (packet.envelope.target != m_name)
        {
            std::cout << m_name << ": forward response has incorrect target\n";
            return;
        }

        // Consume the packet
        m_input->read(packet);
        std::cout
            << m_name << ": forward with atlas uid="
            << m_uid
            << '\n';
    }

    m_processed += processed;
    return;
}

void stateTestForward::Render()
{

}

void stateTestForward::PostUpdate()
{

}

void stateTestForward::Finish()
{
    if (!m_running)
    {
        return;
    }

    std::cout << m_name << ": Finish\n";
    m_running = false;
}