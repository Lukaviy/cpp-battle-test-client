#pragma once

#include "Core/Entity.hpp"
#include "Core/World.hpp"
#include "Features/Common/Actions.hpp"
#include "Features/Common/GroundSpatialComponent.hpp"
#include "Features/Common/HealthComponent.hpp"
#include "Features/Common/MoveTargetComponent.hpp"
#include "Features/Swordsman/RendingAbilityAction.hpp"
#include "IO/Commands/SpawnSwordsman.hpp"
#include "IO/Events/UnitSpawned.hpp"

namespace sw::features::swordsman
{
    // Action priority budget for swordsman. Lower runs first.
    inline constexpr int RendingPriority = 0;
    inline constexpr int MeleePriority = 10;
    inline constexpr int MovePriority = 100;

    inline core::Entity& spawn(core::World& world, const io::SpawnSwordsman& cmd)
    {
        auto& e = world.createEntity(core::EntityId{cmd.unitId});
        e.add<HealthComponent>(cmd.hp);
        e.add<GroundSpatialComponent>();
        e.add<MoveTargetComponent>();
        e.add<RendingAbilityAction>(cmd.chance, cmd.rending, RendingPriority);
        e.add<MeleeAttackAction>(cmd.strength, MeleePriority);
        e.add<MoveToTargetAction>(MovePriority);

        const core::Position pos{cmd.x, cmd.y};
        world.placeEntity(e, pos);
        world.events().event(world.tick(), io::UnitSpawned{cmd.unitId, "Swordsman", cmd.x, cmd.y});
        return e;
    }
}
