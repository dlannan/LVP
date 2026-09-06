#pragma once

#include "module_stream.h"

class module
{
public:
    virtual ~module() = default;

    module(const module&) = delete;
    module& operator=(const module&) = delete;

    virtual bool Init(
        module_stream& input,
        module_stream& output) = 0;

    virtual bool Begin() = 0;

    virtual void PreUpdate() = 0;
    virtual void Update() = 0;
    virtual void PostUpdate() = 0;

    virtual void Finish() = 0;

protected:
    module() = default;

    module_stream* m_input = nullptr;
    module_stream* m_output = nullptr;
};
