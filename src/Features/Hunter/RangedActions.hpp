#pragma once

#include "Core/Combat.hpp"
#include "Core/Component.hpp"
#include "Core/Damage.hpp"
#include "Core/Entity.hpp"
#include "Core/Interfaces.hpp"
#include "Core/World.hpp"
#include "Features/Hunter/PoisonEffect.hpp"
#include "IO/Events/UnitAbilityUsed.hpp"

#include <cstdint>
#include <vector>

namespace sw::features::hunter
{
    // Common precondition: ranged attack is only possible if no other unit is adjacent.
    inline bool noAdjacentEnemies(core::World& world, core::Entity& self, core::Position selfPos)
    {
        std::vector<core::EntityId> neighbours;
        world.queryAdjacent(self.getId(), selfPos, neighbours);
        return neighbours.empty();
    }

    // Poisoned-arrow ability: with chance/1000, instead of the regular ranged shot,
    // applies a stacking-free poison effect that ticks over the next 5 turns.
    class PoisonedArrowAbility : public core::Component<core::ITurnAction>
    {
    public:
        PoisonedArrowAbility(
                std::uint32_t chance, std::uint32_t poison, std::uint32_t minRange, std::uint32_t maxRange, int priority) :
                _chance(chance),
                _poison(poison),
                _minRange(minRange),
                _maxRange(maxRange),
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
            if (!noAdjacentEnemies(world, self, selfPos))
            {
                return false;
            }

            std::vector<core::EntityId> targets;
            world.queryRing(self.getId(), selfPos, _minRange, _maxRange, targets);
            if (targets.empty())
            {
                return false;
            }

            if (!world.random().roll(_chance, 1000))
            {
                return false;
            }

            const auto pick = world.random().uniform(0, static_cast<std::uint32_t>(targets.size() - 1));
            core::Entity* target = world.findEntity(targets[pick]);
            if (target == nullptr)
            {
                return false;
            }

            world.events().event(world.tick(), io::UnitAbilityUsed{self.getId().get_raw(), "POISONED_ARROW"});
            // Replace any previous poison from this hunter (no stacking).
            target->remove<PoisonEffect>();
            target->add<PoisonEffect>(self.getId(), _poison, 5u);
            return true;
        }

    private:
        std::uint32_t _chance{0};
        std::uint32_t _poison{0};
        std::uint32_t _minRange{0};
        std::uint32_t _maxRange{0};
        int _priority{0};
    };

    // Default ranged shot: deals `agility` damage to a random target in [minRange, maxRange]
    // when no other unit is adjacent.
    class RangedAttackAction : public core::Component<core::ITurnAction>
    {
    public:
        RangedAttackAction(
                std::uint32_t agility, std::uint32_t minRange, std::uint32_t maxRange, int priority) :
                _agility(agility),
                _minRange(minRange),
                _maxRange(maxRange),
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
            if (!noAdjacentEnemies(world, self, selfPos))
            {
                return false;
            }

            std::vector<core::EntityId> targets;
            world.queryRing(self.getId(), selfPos, _minRange, _maxRange, targets);
            if (targets.empty())
            {
                return false;
            }

            const auto pick = world.random().uniform(0, static_cast<std::uint32_t>(targets.size() - 1));
            core::Entity* target = world.findEntity(targets[pick]);
            if (target == nullptr)
            {
                return false;
            }

            core::applyDamage(
                    world,
                    *target,
                    core::Damage{self.getId(), target->getId(), _agility, core::DamageKind::Ranged});
            return true;
        }

    private:
        std::uint32_t _agility{0};
        std::uint32_t _minRange{0};
        std::uint32_t _maxRange{0};
        int _priority{0};
    };
}
