#pragma once

#include "state.h"

class state_manager
{
public:
    state * NewState();
    state * GetCurrent();
    state * GetState( std::string state );

    bool Run( void );
    void Init( void );

    // Sibling calls
    void AddSibling(std::string name, std::string ownerstate);
    void CheckSiblingFinish(state * sibstate);
    void ExitSibling(std::string name);

    // General update - passes in mouse px,py and button states by default!
    void Update(int px, int py, int buttons);
    // Render is a frame step. Useful for rendering systems not required.
    void Render();
    // Send message to the current state
    void Message( std::string owner, int message_id, state_packet &message, std::string sender );
    void SendToMessage( std::string target, int message_id, state_packet &message, std::string sender );
    // Input specifically to handle input devices (keyboard, controllers, usb)
    void Input( std::string owner, int action_id, state_action &action );

    // Create a new state to be used in the manager
    void CreateState(std::string name, state *newstate);
    // Change State invokde Finish on the current state and Begin on the Next state.
    void ChangeState(std::string name);
    // A transition is used to control update and render - a separate state will transition between the two states

    void Transition(std::string name);

    // Special local memory for states - not implemented!
    // SetVariable(std::string name, state_data &val);

    // Jump to state moves to another state while maintaining the previous. 
    //   Upon completed the original state is restored and continues.
    //   Finish is not called on current state. Begin and Finish are called on the child state
    void JumpToState(std::string name);

    // Leave the current state calling Finish
    //   Used mostly for child states. If used on root parent, stateManager will exit.
    void ExitState();

    // Special single pass state call -- not fully operational
    void OneShot( state *state, state_callback callback );

    // Set the state manager frame delta (usually before update)
    void SetDelta( double _dt ) { dt = _dt; }

private:
    // -- Index into the states and statenames being used.
    std::string current		= "";
    // Used for transitions
    state *start   	        = nullptr;

    // -- Are the states being executed at the moment, jumped?
    int jumped   	        = 0;

    // Last delta for update/render
    double  dt              = 0.0;

    std::unordered_map<std::string, state *>    states;
    std::vector<std::string>                    stack;
};