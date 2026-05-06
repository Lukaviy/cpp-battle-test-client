#pragma once

#include "Core/Entity.hpp"
#include "Core/World.hpp"
#include "Features/Common/Actions.hpp"
#include "Features/Common/GroundSpatialComponent.hpp"
#include "Features/Common/HealthComponent.hpp"
#include "Features/Common/MoveTargetComponent.hpp"
#include "Features/Hunter/RangedActions.hpp"
#include "IO/Commands/SpawnHunter.hpp"
#include "IO/Events/UnitSpawned.hpp"

namespace sw::features::hunter
{
    // Action priority budget for hunter. Lower runs first.
    // Poisoned arrow takes precedence over the regular ranged shot,
    // which in turn takes precedence over the melee fallback and movement.
    inline constexpr int PoisonedArrowPriority = 0;
    inline constexpr int RangedPriority = 5;
    inline constexpr int MeleePriority = 10;
    inline constexpr int MovePriority = 100;

    inline constexpr std::uint32_t MinRange = 2;

    inline core::Entity& spawn(core::World& world, const io::SpawnHunter& cmd)
    {
        auto& e = world.createEntity(core::EntityId{cmd.unitId});
        e.add<HealthComponent>(cmd.hp);
        e.add<GroundSpatialComponent>();
        e.add<MoveTargetComponent>();
        e.add<PoisonedArrowAbility>(cmd.chance, cmd.poison, MinRange, cmd.range, PoisonedArrowPriority);
        e.add<RangedAttackAction>(cmd.agility, MinRange, cmd.range, RangedPriority);
        e.add<MeleeAttackAction>(cmd.strength, MeleePriority);
        e.add<MoveToTargetAction>(MovePriority);

        const core::Position pos{cmd.x, cmd.y};
        world.placeEntity(e, pos);
        world.events().event(world.tick(), io::UnitSpawned{cmd.unitId, "Hunter", cmd.x, cmd.y});
        return e;
    }
}
