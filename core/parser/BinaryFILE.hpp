#pragma once

#include <cstdint>

struct itchmsg
{
    uint16_t len;
    uint8_t data[0];
};