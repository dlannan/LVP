#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct module_metadata
{
    std::string key;
    std::string value;
};

struct module_envelope
{
    std::uint64_t uid = 0;
    std::uint64_t timestamp = 0;

    std::string owner;
    std::string target;
    std::string memoryScope;
};

struct module_packet
{
    module_envelope envelope;

    std::vector<module_metadata> metadata;

    std::vector<std::uint8_t> payload;
};

