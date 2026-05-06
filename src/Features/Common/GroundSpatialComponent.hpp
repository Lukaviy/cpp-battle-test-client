#pragma once

#include "Core/Component.hpp"
#include "Core/Interfaces.hpp"

namespace sw::features
{
    // Default ground occupancy: blocks the tile it stands on.
    class GroundSpatialComponent : public core::Component<core::ISpatial>
    {
    public:
        [[nodiscard]]
        bool blocksTile() const override
        {
            return true;
        }
    };
}
