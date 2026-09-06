#include "stateForwarding.h"

#include <string>
#include <iostream>

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

    return true;
}

void stateForwarding::PreUpdate()
{

}

void stateForwarding::Update(int px, int py, int buttons)
{

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