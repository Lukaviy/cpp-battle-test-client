#include "RendingEffectComponent.hpp"

#include "PoisonEffectComponent.hpp"
#include "Game/Core/Registrator.hpp"
#include "Game/Core/Simulation/CommandBuffer.hpp"
#include "Game/Core/Event/EventHandlerSystem.hpp"
#include "Game/Features/Events/DamageEvents.hpp"

namespace sw::game::component
{
	void registerRendingEffectComponent(core::Registrator& registrator)
	{
		registrator.handlers().add(
				+[](
				const event::TurnEndTick&,
				const core::EventContext& context,
				core::EntityId entityId,
				RendingEffectComponent&)
				{
					context.commands.removeComponent<RendingEffectComponent>(entityId);
				});

		registrator.handlers().add(
				+[](
				event::CollectReceiveDamageEvent& event,
				const core::EventContext&,
				core::EntityId,
				RendingEffectComponent&,
				PoisonEffectComponent&)
				{
					event.damage.addMultiplier(DamageUnit<DamageKind::Poison>{1000});
				});
	}
}
