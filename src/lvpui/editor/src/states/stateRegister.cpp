#include "stateRegister.h"

#include <string>
#include <iostream>

namespace
{
    constexpr const char* AtlasName = "atlas";

    constexpr const char* RegistrationTarget = "atlas";
    constexpr const char* RegistrationCommand = "register";

    constexpr const char* RegisteredCommand = "registered";
}

stateRegister::stateRegister(std::string name, state_manager *smanager)
    : m_input(nullptr)
    , m_output(nullptr)
    , m_running(false)
    , m_name(name)
    , m_smanager(smanager)
{

}
stateRegister::~stateRegister()
{
    Finish();
}

bool stateRegister::Init(state_stream& input, state_stream& output)  
{
    m_input = &input;
    m_output = &output;

    std::cout << m_name << "_register: Init\n";
    return true;
}

bool stateRegister::Begin()
{
    if (m_input == nullptr || m_output == nullptr)
    {
        return false;
    }

    std::cout << m_name << "_register: Begin\n";

    state_packet packet;

    packet.envelope.uid = 0;
    packet.envelope.owner = m_name.c_str();
    packet.envelope.target = AtlasName;
    packet.envelope.memoryScope = "local";
    packet.metadata.push_back({ "command", "register" });

    if (!m_output->write(packet))
    {
        std::cout << m_name << "_register: failed to queue registration\n";
        return false;
    }

    std::cout << m_name << "_register: registration queued\n";
    m_running = true;
    m_registered = false;

    return true;
}

void stateRegister::PreUpdate()
{

}

void stateRegister::Update(int px, int py, int buttons)
{
    if (!m_running || m_input == nullptr)
        return;

    int processed = 0;

    state_packet packet;

    if(m_input->check(packet))
    {
        ++processed;

        bool isRegistered = false;

        for (const auto& metadata : packet.metadata)
        {
            std::cout << metadata.key << ":" << metadata.value << "\n";
            if (metadata.key == "command" && metadata.value == RegisteredCommand)
            {
                isRegistered = true;
                break;
            }
        }

        if (!isRegistered)
        {
            std::cout << m_name << "_register: ignoring packet\n";
            return;
        }

        if (packet.envelope.target != m_name)
        {
            std::cout << m_name << "_register: registration response has incorrect target\n";
            return;
        }

        if (m_uid != 0)
        {
            std::cout
                << m_name << ": ignoring registration response; "
                << "already registered\n";
            m_registered = true;
        }
        else
        {
            m_input->read(packet);
            m_uid = packet.envelope.uid;

            std::cout
                << m_name << ": registered with atlas uid="
                << m_uid
                << '\n';
            m_registered = true;

            m_smanager->ChangeState("editor_main");            
        }
    }

    m_processed = processed;
    return;
}

void stateRegister::Render()
{

}

void stateRegister::PostUpdate()
{

}

void stateRegister::Finish()
{
    if (!m_running)
    {
        return;
    }

    std::cout << m_name << "_register: Finish\n";
    m_running = false;
}