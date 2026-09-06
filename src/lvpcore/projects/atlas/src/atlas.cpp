#include "atlas.h"

#include <iostream>
#include <string>

namespace
{
    constexpr const char* RegistrationTarget = "atlas";
    constexpr const char* RegistrationCommand = "register";
}

atlas::atlas()
    : m_input(nullptr)
    , m_output(nullptr)
    , m_running(false)
    , m_nextUid(1)
{
}

atlas::~atlas()
{
    Finish();
}

bool atlas::Init(module_stream& input, module_stream& output)
{
    m_input = &input;
    m_output = &output;

    std::cout << "atlas: Init\n";

    return true;
}

bool atlas::Begin()
{
    if (m_input == nullptr || m_output == nullptr)
    {
        return false;
    }

    std::cout << "atlas: Begin\n";

    m_running = true;

    return true;
}

void atlas::PreUpdate()
{
    // Reserved for future pre-update processing.
}

int atlas::Update()
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

        const module_uid uid = m_nextUid++;

        m_modules.emplace(
            uid,
            packet.envelope);

        std::cout
            << "atlas: registration request from "
            << packet.envelope.owner
            << " assigned uid="
            << uid
            << '\n';

        module_packet response;

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

    return processed;
}

void atlas::PostUpdate()
{
    // Reserved for future post-update processing.
}

void atlas::Finish()
{
    if (!m_running)
    {
        return;
    }

    std::cout << "atlas: Finish\n";

    m_running = false;
}