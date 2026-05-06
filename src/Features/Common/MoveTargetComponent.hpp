#pragma once

#include "Core/Component.hpp"
#include "Core/Interfaces.hpp"
#include "Core/Position.hpp"

#include <optional>

namespace sw::features
{
    // Holds the desired march target for action priorities.
    class MoveTargetComponent : public core::DataComponent
    {
    public:
        [[nodiscard]]
        bool hasTarget() const noexcept
        {
            return _target.has_value();
        }

        [[nodiscard]]
        core::Position target() const noexcept
        {
            return _target.value_or(core::Position{});
        }

        void setTarget(core::Position p) noexcept
        {
            _target = p;
        }

        void clear() noexcept
        {
            _target.reset();
        }

    private:
        std::optional<core::Position> _target;
    };
}
