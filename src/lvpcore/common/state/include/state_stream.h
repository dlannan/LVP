#pragma once

#include "state_packet.h"

class state_stream
{
public:
    virtual ~state_stream();

    state_stream(const state_stream&) = delete;
    state_stream& operator=(const state_stream&) = delete;

    virtual bool available() const = 0;

    virtual bool check(state_packet& packet) = 0;

    virtual bool read(state_packet& packet) = 0;

    virtual bool write(const state_packet& packet) = 0;

protected:
    state_stream() = default;
};
