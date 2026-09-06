#include "stateControl.h"

#include <string>
#include <iostream>

stateControl::stateControl() 
{

}
stateControl::~stateControl()
{

}

bool stateControl::Init(state_stream& input, state_stream& output)  
{
    m_input = &input;
    m_output = &output;
    return true;
}

bool stateControl::Begin()
{

    return true;
}

void stateControl::PreUpdate()
{

}

void stateControl::Update(int px, int py, int buttons) 
{

}

void stateControl::Render()
{

}

void stateControl::PostUpdate() 
{

}

void stateControl::Finish()
{

}