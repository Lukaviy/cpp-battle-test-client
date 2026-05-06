#pragma once

#include "Core/Entity.hpp"
#include "Core/World.hpp"
#include "Features/Common/MoveTargetComponent.hpp"
#include "IO/Commands/March.hpp"
#include "IO/Events/MarchStarted.hpp"

namespace sw::features
{
    inline void handleMarch(core::World& world, const io::March& cmd)
    {
        core::Entity* e = world.findEntity(core::EntityId{cmd.unitId});
        if (e == nullptr)
        {
            return;
        }
        auto* mt = e->get<MoveTargetComponent>();
        if (mt == nullptr)
        {
            mt = &e->add<MoveTargetComponent>();
        }
        const core::Position target{cmd.targetX, cmd.targetY};
        mt->setTarget(target);

        core::Position from;
        world.map().tryGetPosition(e->getId(), from);
        world.events().event(
                world.tick(), io::MarchStarted{cmd.unitId, from.x, from.y, target.x, target.y});
    }
}
