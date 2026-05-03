#include "HealthComponent.hpp"

#include "Game/Core/Simulation/CommandBuffer.hpp"
#include "Game/Core/Event/EventHandlerSystem.hpp"
#include "Game/Features/Events/DamageEvents.hpp"
#include "Game/Core/Registrator.hpp"

namespace sw::game::component
{
	void registerHealthComponent(core::Registrator& registrator)
	{
		registrator.handlers().add(
				+[](
				const event::ReceiveDamageEvent& event,
				const core::EventContext& context,
				core::EntityId entityId,
				HealthComponent& health)
				{
					const auto damage = totalDamage(event.damage);

					health.health = std::max(0, health.health - damage);

					if (health.health <= 0)
					{
						context.commands.removeEntity(entityId);
					}
				});
	}
}
