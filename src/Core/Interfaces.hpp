#pragma once

#include "Component.hpp"
#include "Damage.hpp"

#include <cstdint>

namespace sw::core
{
    class World;
    class Entity;

    // One per-turn action attempt. Higher priority runs first; first action that
    // returns true consumes the unit's turn.
    struct ITurnAction
    {
        virtual ~ITurnAction() = default;

        [[nodiscard]]
        virtual int priority() const = 0;

        // Returns true if the turn was consumed.
        virtual bool tryExecute(World& world, Entity& self) = 0;
    };

    // HP carrier.
    struct IHealth
    {
        virtual ~IHealth() = default;

        [[nodiscard]]
        virtual std::uint32_t current() const = 0;
        [[nodiscard]]
        virtual bool isAlive() const = 0;

        virtual void applyDamage(std::uint32_t amount) = 0;
    };

    // Mutates incoming damage before it is applied. Used by armor, vulnerabilities,
    // "rending doubles poison this turn", etc.
    struct IDamageModifier
    {
        virtual ~IDamageModifier() = default;

        virtual void modifyIncoming(Damage& damage) = 0;
    };

    // Pure observer of damage already applied. Used by logs, on-hit triggers.
    struct IDamageObserver
    {
        virtual ~IDamageObserver() = default;

        virtual void onDamageReceived(const Damage& damage) = 0;
    };

    // Notified at the start of the owner's own turn. Used by ticking effects (poison).
    struct ITurnTick
    {
        virtual ~ITurnTick() = default;

        virtual void onTurnTick(World& world, Entity& self) = 0;
    };

    // Spatial occupancy contract: by default a unit occupies its single tile and
    // blocks pathing for others. Flying entities can implement it as a no-op.
    struct ISpatial
    {
        virtual ~ISpatial() = default;

        [[nodiscard]]
        virtual bool blocksTile() const = 0;
    };
}
