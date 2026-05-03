#include "PoisonEffectComponent.hpp"

#include "HealthComponent.hpp"
#include "Game/Core/Simulation/CommandBuffer.hpp"
#include "Game/Core/Event/EventHandlerSystem.hpp"
#include "Game/Features/Behaviours/Helpers/Attack.hpp"
#include "Game/Core/Registrator.hpp"

namespace sw::game::component
{
	namespace
	{
		void tick(
				const event::TurnEndTick&,
				const core::EventContext& context,
				core::EntityId entityId,
				PoisonEffectComponent& poison_effect)
		{
			const auto damageThisTick = std::min(poison_effect.damagePerTick, poison_effect.remainingDamage);

			behaviour::receiveDamage(
					context, entityId, poison_effect.sourceEntity, DamageUnit<DamageKind::Poison>{damageThisTick});

			poison_effect.remainingDamage -= damageThisTick;

			if (poison_effect.remainingDamage <= 0)
			{
				context.commands.removeComponent<PoisonEffectComponent>(entityId);
			}
		}
	}

	PoisonEffectComponent PoisonEffectComponent::fromTotalDamage(
			core::EntityId sourceEntity,
			std::int32_t totalDamage,
			std::int32_t tickCount)
	{
		return PoisonEffectComponent{
				.sourceEntity = sourceEntity,
				.remainingDamage = totalDamage,
				.damagePerTick = (totalDamage + tickCount - 1) / tickCount
		};
	}

	void registerPoisonEffectComponent(core::Registrator& registrator)
	{
		registrator.handlers().add(
				+[](
				const event::AddExisting<PoisonEffectComponent>& event,
				const core::EventContext&,
				core::EntityId,
				const PoisonEffectComponent& poison_effect)
				{
					event.component.remainingDamage += poison_effect.remainingDamage;
					event.component.damagePerTick
							= std::max(event.component.damagePerTick, poison_effect.damagePerTick);
				});

		registrator.handlers().add(tick);
	}
}
