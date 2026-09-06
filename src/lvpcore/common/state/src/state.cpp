
#include "../include/state_stream.h"

#include "state.h"

state_stream::~state_stream() = default;


bool state::Begin() { return true; }

bool state::Init(state_stream& input, state_stream& output) {
    m_input     = &input; 
    m_output    = &output; 
    return true;
}

void state::PreUpdate()
{

}

void state::Update(int px, int py, int buttons)
{

}

void state::Render()
{

}

void state::PostUpdate()
{

}

void state::Message(std::string owner, int message_id, state_packet &message, std::string sender )
{

}

void state::Input(std::string owner, int action_id, state_action &action )
{

}

void state::Finish() {}