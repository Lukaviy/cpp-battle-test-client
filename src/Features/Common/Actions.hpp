#pragma once

#include "Core/Combat.hpp"
#include "Core/Component.hpp"
#include "Core/Damage.hpp"
#include "Core/Entity.hpp"
#include "Core/Interfaces.hpp"
#include "Core/World.hpp"
#include "Features/Common/MoveTargetComponent.hpp"
#include "IO/Events/MarchEnded.hpp"
#include "IO/Events/UnitMoved.hpp"

#include <cstdint>
#include <vector>

namespace sw::features
{
    // Picks a random adjacent enemy and deals `strength` melee damage.
    // Priority is configurable so other actions can sit before/after it.
    class MeleeAttackAction : public core::Component<core::ITurnAction>
    {
    public:
        MeleeAttackAction(std::uint32_t strength, int priority) :
                _strength(strength),
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

            const auto pick = world.random().uniform(0, static_cast<std::uint32_t>(neighbours.size() - 1));
            core::Entity* target = world.findEntity(neighbours[pick]);
            if (target == nullptr)
            {
                return false;
            }

            core::applyDamage(world, *target, core::Damage{self.getId(), target->getId(), _strength, core::DamageKind::Melee});
            return true;
        }

    private:
        std::uint32_t _strength{0};
        int _priority{0};
    };

    // Steps one tile toward a stored march target, if any.
    class MoveToTargetAction : public core::Component<core::ITurnAction>
    {
    public:
        explicit MoveToTargetAction(int priority) :
                _priority(priority)
        {}

        [[nodiscard]]
        int priority() const override
        {
            return _priority;
        }

        bool tryExecute(core::World& world, core::Entity& self) override
        {
            auto* mt = self.get<MoveTargetComponent>();
            if (mt == nullptr || !mt->hasTarget())
            {
                return false;
            }

            core::Position from;
            if (!world.map().tryGetPosition(self.getId(), from))
            {
                return false;
            }
            const core::Position target = mt->target();

            if (core::Position next; world.map().tryStepToward(from, target, next))
            {
                world.moveEntity(self, next);
                world.events().event(world.tick(), io::UnitMoved{self.getId().get_raw(), next.x, next.y});
                if (next == target)
                {
                    mt->clear();
                    world.events().event(world.tick(), io::MarchEnded{self.getId().get_raw(), next.x, next.y});
                }
                return true;
            }
            // Target unreachable this tick. Still consume the turn to avoid spinning.
            return false;
        }

    private:
        int _priority{0};
    };
}
