#pragma once

#include "Core/Combat.hpp"
#include "Core/Component.hpp"
#include "Core/Damage.hpp"
#include "Core/Entity.hpp"
#include "Core/Interfaces.hpp"
#include "Core/World.hpp"
#include "IO/Events/UnitAbilityUsed.hpp"

#include <cstdint>
#include <vector>

namespace sw::features::swordsman
{
    // Rending Strike: with chance/1000 deal `rending` damage to a random adjacent enemy
    // instead of the regular melee swing. Sits at a higher priority than the melee action.
    class RendingAbilityAction : public core::Component<core::ITurnAction>
    {
    public:
        RendingAbilityAction(std::uint32_t chance, std::uint32_t rending, int priority) :
                _chance(chance),
                _rending(rending),
                _priority(priority)
        {}

        [[nodiscard]]
        int priority() const override
        {
            return _priority;
        }

        bool tryExecute(core::World& world, core::Entity& self) override
        {
            core::Position selfPos;
            if (!world.map().tryGetPosition(self.getId(), selfPos))
            {
                return false;
            }

            std::vector<core::EntityId> neighbours;
            world.queryAdjacent(self.getId(), selfPos, neighbours);
            if (neighbours.empty())
            {
                return false;
            }

            if (!world.random().roll(_chance, 1000))
            {
                return false;
            }

            const auto pick = world.random().uniform(0, static_cast<std::uint32_t>(neighbours.size() - 1));
            core::Entity* target = world.findEntity(neighbours[pick]);
            if (target == nullptr)
            {
                return false;
            }

            world.events().event(world.tick(), io::UnitAbilityUsed{self.getId().get_raw(), "RENDING"});
            core::applyDamage(
                    world, *target, core::Damage{self.getId(), target->getId(), _rending, core::DamageKind::Rending});
            return true;
        }

    private:
        std::uint32_t _chance{0};
        std::uint32_t _rending{0};
        int _priority{0};
    };
}
