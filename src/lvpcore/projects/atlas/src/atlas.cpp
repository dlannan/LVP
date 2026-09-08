#include "atlas.h"

#include <iostream>
#include <string>

// Atlas 
//   Atlas is like a dns service for processes. 
//   Processes register with atlas, and then all send/recv from process <-> process occurs via atlas.
//   All data streaming is transport agnostic: meaning that this data can be moved via a file stream,
//      a memory stream, or a network stream. As far as atlas knows its all the same.
//   Benefits: Video/Audio/Data can all use realtime, or offline data streams without any change in 
//      process development. Thus a plugin made externally works regardless of the type of transport.
//
//   Operation:
//      1. Atlas listens to register/unregister commands and adds/removes from its table.
//      2. Listens to routing commands and forwards streams to the correct process handles.
//      3. Listens to Zeus for command and control operations.



atlas::atlas()
    : m_input(nullptr)
    , m_output(nullptr)
    , m_running(false)
{
}

atlas::~atlas()
{
    Finish();
}

bool atlas::Init(state_stream& input, state_stream& output)
{
    m_input = &input;
    m_output = &output;

    m_registration.Init(input, output);
    m_forwarding.Init(input, output);

    m_smanager.Init();
    m_smanager.CreateState("atlas_registration", &m_registration);
    m_smanager.CreateState("atlas_forwarder", &m_forwarding);
    
    std::cout << "atlas: Init\n";
    return true;
}

bool atlas::Begin()
{
    m_smanager.ChangeState("atlas_registration");
    m_smanager.AddSibling("atlas_forwarder", "atlas_registration");
    return true;
}
 
void atlas::PreUpdate()
{
    // Reserved for future pre-update processing.
}

void atlas::Update()
{
    m_smanager.Update(0, 0, 0);
    m_smanager.Render();
    return;
}

void atlas::PostUpdate()
{
    // Reserved for future post-update processing.
}

void atlas::Finish()
{
    m_registration.Finish();
}