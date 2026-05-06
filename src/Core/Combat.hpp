#pragma once

#include "Damage.hpp"
#include "Entity.hpp"
#include "IO/Events/UnitDied.hpp"
#include "IO/System/EventSystem.hpp"
#include "Interfaces.hpp"

namespace sw::core
{
    class World;

    // Single entry point for all damage in the simulation. Runs through the modifier
    // pipeline, applies HP delta, runs observers, accumulates the per-attacker total
    // for the UNIT_ATTACKED event, and emits UNIT_DIED on lethal damage.
    void applyDamage(World& world, Entity& target, Damage damage);
}
