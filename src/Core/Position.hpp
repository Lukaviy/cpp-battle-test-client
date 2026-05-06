#pragma once

#include <cstdint>

namespace sw::core
{
    struct Position
    {
        uint32_t x{0};
        uint32_t y{0};

        friend constexpr bool operator==(const Position& a, const Position& b) noexcept
        {
            return a.x == b.x && a.y == b.y;
        }
    };
}
