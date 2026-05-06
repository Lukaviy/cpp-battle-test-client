#pragma once

#include "Ids.hpp"

#include <cstdint>

namespace sw::core
{
    enum class DamageKind : std::uint8_t
    {
        Generic,
        Melee,
        Ranged,
        Rending,
        Poison
    };

    struct Damage
    {
        EntityId attackerId{0};
        EntityId targetId{0};
        std::uint32_t amount{0};
        DamageKind kind{DamageKind::Generic};
    };
}
