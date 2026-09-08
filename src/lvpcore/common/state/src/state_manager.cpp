

// ----------------------------------------------------------------
// -- Simple State Manager.
// ----------------------------------------------------------------

// Notes:
//  - State Manager has no memory responsibility over states - you are responsible!
//  - States can be anyting as long as it adheres to the state iface.
//  - For state sharing its recommended to create messages or handles that states can access.

#include "state_manager.h"

#include <string>
#include <iostream>

namespace {
    state      defaultstate   = {};
};

// ----------------------------------------------------------------

state *state_manager::NewState()
{
	return new state();
}

// ----------------------------------------------------------------
// -- Create the stateManager
void state_manager::Init()
{
    states.clear();
    stack.clear();
}

// ----------------------------------------------------------------

state *state_manager::GetCurrent()
{
    if(current == "" || current.empty()) return nullptr;
    return states[current];
}

// ----------------------------------------------------------------

state *state_manager::GetState( std::string state )
{
    if(state == "" || state.empty() ) return nullptr;
    return states[state];
}

// ----------------------------------------------------------------

bool state_manager::Run()
{
    if( current.empty() ) return true;
    const auto *state = states[current];
    return state->run;
}

// ----------------------------------------------------------------
// -- TODO: Cleanup this sibling SHIT its not nice.
// -- Create a new state then return the state object
void state_manager::AddSibling(std::string name, std::string ownerstate)
{
    if( states.contains(ownerstate) && states.contains(name)) {         
        states[ownerstate]->siblings[name] = new state_sibling{ name, ownerstate };
        auto *name_state = states[name];
        name_state->owner = ownerstate;
        name_state->Begin();
    }
    else 
        std::cout << "[Error] Cannot add sibling: " << name <<"  to " << ownerstate << "\n";
}

// ----------------------------------------------------------------

void state_manager::CheckSiblingFinish(state *sibstate)
{
    // -- If exiting a sibling, then make sure the owner has it cleared!
    if(!sibstate->owner.empty()) {
        states.at(sibstate->owner)->siblings.erase(sibstate->name());
    }
}

// ----------------------------------------------------------------

void state_manager::ExitSibling(std::string name)
{
    if( !states.contains(name)) {
        std::cout << "[Error] Cannot remove sibling: " << name << "\n";
        return;
    }

    auto * sstate = states[name];
    if( states.contains(sstate->owner)) {
        CheckSiblingFinish(sstate);
        sstate->owner = "";
        sstate->Finish();
        sstate->init = false;
    }     

}

// ----------------------------------------------------------------
// -- On update manage the current states
void state_manager::Update(int px, int py, int buttons)
{
    if(current.empty()) return;
    if(states.contains(current))
    {
        auto *curr_state = states[current];
        curr_state->self  = this;
        curr_state->dt    = dt;

        // -- If this state has siblings run them order is unknown
        for (const auto& [sname, sibs] : curr_state->siblings) {
            if(states.contains(sibs->name)) {
                auto *sibling = states.at(sibs->name);
                sibling->self  = this;
                sibling->dt    = dt;
                sibling->Update(px, py, buttons);
            }
        }

        curr_state->Update(px, py, buttons);
    }
}

// ----------------------------------------------------------------
// -- On update manage the current states
void state_manager::Render()
{
    if(current.empty()) return;
    if(states.contains(current))
    {
        auto *curr_state = states[current];
        curr_state->self  = this;
        curr_state->dt    = dt;

        // -- If this state has siblings run them in order _after_ before this state
        for (const auto& [sname, sibs] : curr_state->siblings) {
            if(states.contains(sibs->name)) {
                auto *sibling = states.at(sibs->name);
                sibling->self  = this;
                sibling->dt    = dt;
                sibling->Render();
            }
        }

        curr_state->Render();
    }
}

// ----------------------------------------------------------------
// -- On message send to the current state
void state_manager::Message( std::string owner, int message_id, state_packet &message, std::string sender )
{
    if(current.empty()) return;
    if(states.contains(current)) 
    {
        auto *curr_state = states[current];
        if( !curr_state->name().empty() ) {
            // -- If this state has siblings run them in order _after_ before this state
            for (const auto& [sname, sibs] : curr_state->siblings) {
                if(states.contains(sibs->name)) {
                    states.at(sibs->name)->Message(owner, message_id, message, sender );
                }
            }

            curr_state->Message( owner, message_id, message, sender );
        }
    }
}

// ----------------------------------------------------------------
// -- On message send to the current state
void state_manager::SendToMessage( std::string target, int message_id, state_packet &message, std::string sender )
{
    if(target.empty()) return;
    if(states.contains(target)) {
        auto *curr_state = states[current];
        if( !curr_state->name().empty() ) {
            curr_state->Message( target, message_id, message, sender );
        }
    }
}

// ----------------------------------------------------------------
// -- On input send to the current state
void state_manager::Input( std::string owner, int action_id, state_action &action )
{
    if(current.empty()) return;
    if(states.contains(current))
    {
        auto *curr_state = states[current];
        if( !curr_state->name().empty() ) {
            // -- If this state has siblings run them in order _after_ before this state
            for (const auto& [sname, sibs] : curr_state->siblings) {
                if(states.contains(sibs->name)) {
                    states.at(sibs->name)->Input(owner, action_id, action );   
                }
            }

            curr_state->Input( owner, action_id, action );
        }
    }
}

// ----------------------------------------------------------------
// -- Create a new state then return the state object
void state_manager::CreateState(std::string name, state *newstate)
{
    newstate->SetName( name );
    states[name] = newstate;
}

// ----------------------------------------------------------------
// -- A ChangeState invokes an End on the current state, And 
// -- a Begin on the Next state.

void state_manager::ChangeState(std::string name)
{
    std::cout << "Changing State to... " << name << "\n";
    if(!states.contains(name)) {
        std::cout << "Error: Invalid State Name: " << name << "\n";
        return;
    }

    if(start == nullptr) {
        current = name;
        start = states.at(current);
    }
    else
    {
        state *curr_state =  states.at(current);
        curr_state->self  = this;

        // -- If this state has siblings run them in order _after_ before this state
        for (const auto& [sname, sibs] : curr_state->siblings) {
            if(states.contains(sibs->name)) {
               states.at(sibs->name)->Finish();
            }
        }

        curr_state->Finish();
        curr_state->init = false;
    }

    current = name;

    auto *curr_state = states.at(current);
    curr_state->self  = this;
    curr_state->once  = 0;
    curr_state->Begin();

    // -- If this state has siblings run them in order _after_ before this state
    for (const auto& [sname, sibs] : curr_state->siblings) {
        if(states.contains(sibs->name)) {
            states.at(sibs->name)->Begin();
        }
    }
    curr_state->init = true;
}

// ----------------------------------------------------------------
// -- A Transition is used to control Update and Render
// --   Another state will transition between two states

void state_manager::Transition(std::string name)
{
    std::cout << "Transitioning State to... " << name << "\n";
    if(!states.contains(name)) {
        std::cout << "Error: Invalid State Name: " << name << "\n";
        return ;
    }

    if(start == nullptr) {
        current = name;
        start = states.at(current);
    }
    else
        states[current]->self  = this;

    current = name;
    auto *curr_state = states[current];
    curr_state->self  = this;
}

// ----------------------------------------------------------------
// --  SetVariable
// -- Allows the setting of state variables - like sound, level etc.
// -- Some special var names set profile information

// void state_manager::SetVariable(name, val)
// {
// TBD
// }

// ----------------------------------------------------------------
// -- JumpToState moves To another state While maintaining, the
// --  previous one. Once complete, a jumped state will Return To
// --  the original owner of the child state.

// -- End is Not called on the current state, Begin And End
// -- are called on the child state.

void state_manager::JumpToState(std::string name)
{
    std::cout << "Jumping State to... " << name;
    
    if(!states.contains(name)) {
        std::cout << "Error: Invalid State Name: " << name << "\n";
        return;
    }

    // -- Entering jump state - could be multiple depth, cannot Exit
    // -- Until stack is correctly depleted (Or stack out of balance)
    if(jumped == 0) {
        jumped = 1;
        stack.clear();
    } else {
        jumped++;
    }
    // -- Alreay in jump state, Then keep going...
    // -- Put current onto stack
    stack[jumped] = current;

    // -- Enter New state  
    current = name;
    auto *curr_state = states[current];
    curr_state->self  = this;
    curr_state->once  = 0;

    // -- If this state has siblings run them in order _after_ before this state
    for (const auto& [sname, sibs] : curr_state->siblings) {
        if(states.contains(sname)) {
            states.at(sname)->Begin();
        }
    }

    curr_state->Begin();
    curr_state->init = true;
}

// ----------------------------------------------------------------
// -- Exit out of the current Jumped state

void state_manager::ExitState()
{
    // -- If exiting a normal state, then set run to false, call Finish and 
    // -- exit state manager (likely)
    if(jumped == 0) {
        auto *curr_state = states[current];
        curr_state->run = false;
        curr_state->self  = this;

        // -- If this state has siblings run them in order _after_ before this state
        for (const auto& [sname, sibs] : curr_state->siblings) {
            if(states.contains(sibs->name)) {
                states.at(sibs->name)->Finish();
            }
        }
        curr_state->Finish();
        curr_state->init = false;

        std::cout << "Exiting state..." << current << "\n";
        return;
    }

    // -- Examine stack To see If there is a valid jumpstate on it
    // -- If there is only one left, this is the normal parent, so Return
    // -- sensibly To it, otherwise pop the stack And change
    if(jumped > 0) {
        auto *curr_state = states[current];
        curr_state->self  = this;

        // -- If this state has siblings run them in order _after_ before this state
        for (const auto& [sname, sibs] : curr_state->siblings) {
            if(states.contains(sibs->name)) 
                states.at(sibs->name)->Finish();
        }
        curr_state->Finish();
        curr_state->init = false;

        std::cout << "Exiting state..." << current << "\n";
        auto name = stack[jumped];

        if(states.contains(name)) current = name;
        jumped --;
    }
}

// ----------------------------------------------------------------
// -- Call a function just once (until next begin)
void state_manager::OneShot( state *name_state, state_callback callback )
{
    if(name_state->once == 0) {
        name_state->once  = 1;
        if(callback) callback();
    }
}

// ----------------------------------------------------------------


