#include "module.h"

#include <iostream>

namespace
{
    constexpr const char* DummyName = "dummy";
    constexpr const char* AtlasName = "atlas";

    constexpr const char* RegisteredCommand = "registered";
}

dummy::dummy()
    : m_input(nullptr)
    , m_output(nullptr)
    , m_uid(0)
    , m_running(false)
{
}

dummy::~dummy()
{
    Finish();
}

bool dummy::Init( module_stream& input, module_stream& output)
{
    m_input = &input;
    m_output = &output;

    std::cout << "dummy: Init\n";
    return true;
}

bool dummy::Begin()
{
    if (m_input == nullptr ||
        m_output == nullptr)
    {
        return false;
    }

    std::cout << "dummy: Begin\n";

    module_packet packet;

    packet.envelope.uid = 0;
    packet.envelope.owner = DummyName;
    packet.envelope.target = AtlasName;
    packet.envelope.memoryScope = "local";
    packet.metadata.push_back({ "command", "register" });

    if (!m_output->write(packet))
    {
        std::cout << "dummy: failed to queue registration\n";
        return false;
    }

    std::cout << "dummy: registration queued\n";
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

    module_packet packet;

    while (m_input->read(packet))
    {
        ++processed;

        bool isRegistered = false;

        for (const auto& metadata : packet.metadata)
        {
            if (metadata.key == "command" &&
                metadata.value == RegisteredCommand)
            {
                isRegistered = true;
                break;
            }
        }

        if (!isRegistered)
        {
            std::cout << "dummy: ignoring packet\n";
            continue;
        }

        if (packet.envelope.target != DummyName)
        {
            std::cout << "dummy: registration response has incorrect target\n";
            continue;
        }

        if (m_uid != 0)
        {
            std::cout
                << "dummy: ignoring registration response; "
                << "already registered\n";
            continue;
        }

        m_uid = packet.envelope.uid;

        std::cout
            << "dummy: registered with atlas uid="
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

    std::cout << "dummy: Finish\n";
    m_running = false;
}
