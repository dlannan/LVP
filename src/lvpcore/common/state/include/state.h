#pragma once

#include "state_stream.h"

#include <cstdint>
#include <unordered_map>

struct state_sibling
{
    std::string     name;
    std::string     owner;
};

struct state_action
{
    std::string     name;
    std::string     action;
    std::string     data;
};

typedef void (*state_callback)(void);

class state;

using state_preupdate_callback = void (*)();
using state_update_callback = int (*)(int, int, int);
using state_render_callback = void (*)();
using state_postupdate_callback = void (*)();

using state_message_callback = void (*)(std::string, int, state_packet&, std::string);
using state_input_callback = void (*)(std::string, int, state_action&);

class state
{
public:
    state() {}
    ~state() {}

    state(const state&) {}
    state(const char *name): m_name(name) {}
//    state& operator=(const state&) { return state == this; }

    virtual bool Init(state_stream& input, state_stream& output);
    virtual bool Begin();

    virtual void PreUpdate();
    virtual void Update(int px, int py, int buttons);
    virtual void Render();
    virtual void PostUpdate();

    virtual void Message(std::string owner, int message_id, state_packet &message, std::string sender );
    virtual void Input(std::string owner, int action_id, state_action &action );

    virtual void Finish();

    std::string name() { return m_name; }
    void SetName(std::string name) { m_name = name; }

public:
    std::unordered_map<std::string, state_sibling *> siblings;
    std::string     owner;

    double dt       = 0.0;

    bool init       = false;
    bool run        = true;

    int once        = 0;

    void *self      = nullptr;

protected:

    state_stream*   m_input = nullptr;
    state_stream*   m_output = nullptr;

    std::uint64_t   m_uid;
    std::string     m_name;

    bool            m_running;    
};
