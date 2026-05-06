#include "Combat.hpp"

#include "World.hpp"

namespace sw::core
{
    void applyDamage(World& world, Entity& target, Damage damage)
    {
        damage.targetId = target.getId();

        // Modifier pipeline: armor, vulnerabilities, "rending doubles poison this turn", etc.
        target.emit<&IDamageModifier::modifyIncoming>(damage);

        auto* health = target.get<IHealth>();
        if (health == nullptr)
        {
            return;
        }

        health->applyDamage(damage.amount);
        const auto hpAfter = health->current();

        // Observers (e.g. logs, on-hit triggers).
        target.emit<&IDamageObserver::onDamageReceived>(damage);

        // Aggregate for the per-tick UNIT_ATTACKED event.
        world.damageAccumulator().add(damage, hpAfter);

        if (!health->isAlive())
        {
            world.events().event(world.tick(), io::UnitDied{target.getId().get_raw()});
            world.destroyEntity(target.getId());
        }
    }
}
