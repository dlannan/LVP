#pragma once

#include "module_packet.h"

class module_stream
{
public:
    virtual ~module_stream();

    module_stream(const module_stream&) = delete;
    module_stream& operator=(const module_stream&) = delete;

    virtual bool available() const = 0;

    virtual bool read(module_packet& packet) = 0;

    virtual bool write(const module_packet& packet) = 0;

protected:
    module_stream() = default;
};
