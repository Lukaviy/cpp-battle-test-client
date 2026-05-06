#pragma once

#include "Core/Combat.hpp"
#include "Core/Component.hpp"
#include "Core/Damage.hpp"
#include "Core/Entity.hpp"
#include "Core/Interfaces.hpp"
#include "Core/World.hpp"

#include <cstdint>

namespace sw::features::hunter
{
    // Damage-over-time effect attached to the poisoned target.
    // Total `total` damage is split across `ticks` ticks; on a tick when the target
    // also received Rending damage in the same turn, this tick's damage is doubled
    // (without changing the total budget).
    class PoisonEffect : public core::Component<core::ITurnTick, core::IDamageObserver>
    {
    public:
        PoisonEffect(core::EntityId source, std::uint32_t total, std::uint32_t ticks) :
                _source(source),
                _remaining(total),
                _ticksLeft(ticks)
        {}

        void onTurnTick(core::World& world, core::Entity& self) override
        {
            if (_ticksLeft == 0 || _remaining == 0)
            {
                self.remove<PoisonEffect>();
                return;
            }

            const std::uint32_t base = _remaining / _ticksLeft;
            const std::uint32_t amount = _rendingThisTurn ? base * 2u : base;

            _remaining = base >= _remaining ? 0u : _remaining - base;
            --_ticksLeft;
            _rendingThisTurn = false;

            core::applyDamage(
                    world, self, core::Damage{_source, self.getId(), amount, core::DamageKind::Poison});

            if (_ticksLeft == 0 || _remaining == 0)
            {
                self.remove<PoisonEffect>();
            }
        }

        void onDamageReceived(const core::Damage& damage) override
        {
            if (damage.kind == core::DamageKind::Rending)
            {
                _rendingThisTurn = true;
            }
        }

    private:
        core::EntityId _source{0};
        std::uint32_t _remaining{0};
        std::uint32_t _ticksLeft{0};
        bool _rendingThisTurn{false};
    };
}
