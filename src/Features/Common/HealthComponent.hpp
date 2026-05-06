#pragma once

#include "Core/Component.hpp"
#include "Core/Interfaces.hpp"

#include <cstdint>

namespace sw::features
{
    // Default HP carrier. Replace with another IHealth implementation
    // (e.g. InvulnerableHealth) to change behavior without touching call sites.
    class HealthComponent : public core::Component<core::IHealth>
    {
    public:
        explicit HealthComponent(std::uint32_t hp) :
                _hp(hp)
        {}

        [[nodiscard]]
        std::uint32_t current() const override
        {
            return _hp;
        }

        [[nodiscard]]
        bool isAlive() const override
        {
            return _hp > 0;
        }

        void applyDamage(std::uint32_t amount) override
        {
            _hp = amount >= _hp ? 0u : _hp - amount;
        }

    private:
        std::uint32_t _hp{0};
    };
}
