#include "state.h"

#include <iostream>

namespace
{
    constexpr const char* DummyName = "dummy";
    constexpr const char* AtlasName = "atlas";

    constexpr const char* RegisteredCommand = "registered";
}

dummy::dummy( const char * name = DummyName )
    : m_input(nullptr)
    , m_output(nullptr)
    , m_uid(0)
    , m_running(false)
    , m_name(name)
{
}

dummy::~dummy()
{
    Finish();
}

bool dummy::Init( state_stream& input, state_stream& output)
{
    m_input = &input;
    m_output = &output;

    std::cout << m_name << ": Init\n";
    return true;
}

bool dummy::Begin()
{
    if (m_input == nullptr || m_output == nullptr)
    {
        return false;
    }

    std::cout << m_name << ": Begin\n";

    state_packet packet;

    packet.envelope.uid = 0;
    packet.envelope.owner = m_name.c_str();
    packet.envelope.target = AtlasName;
    packet.envelope.memoryScope = "local";
    packet.metadata.push_back({ "command", "register" });

    if (!m_output->write(packet))
    {
        std::cout << m_name << ": failed to queue registration\n";
        return false;
    }

    std::cout << m_name << ": registration queued\n";
    m_running = true;

    return true;
}

void dummy::PreUpdate()
{
    // Reserved for future pre-update processing.
}

int dummy::Update()
{
    if (!m_running || m_input == nullptr)
    {
        return 0;
    }

    int processed = 0;

    state_packet packet;

    while (m_input->read(packet))
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
            std::cout << m_name << ": ignoring packet\n";
            continue;
        }

        if (packet.envelope.target != m_name)
        {
            std::cout << m_name << ": registration response has incorrect target\n";
            continue;
        }

        if (m_uid != 0)
        {
            std::cout
                << m_name << ": ignoring registration response; "
                << "already registered\n";
            continue;
        }

        m_uid = packet.envelope.uid;

        std::cout
            << m_name << ": registered with atlas uid="
            << m_uid
            << '\n';
    }

    return processed;
}

void dummy::PostUpdate()
{
    // Reserved for future post-update processing.
}

void dummy::Finish()
{
    if (!m_running)
    {
        return;
    }

    std::cout << m_name << ": Finish\n";
    m_running = false;
}
