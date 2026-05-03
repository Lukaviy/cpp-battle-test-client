#include "HunterBehaviour.hpp"

#include "Helpers/Attack.hpp"
#include "Helpers/DistanceAttack.hpp"
#include "Helpers/MeleeAttack.hpp"
#include "Helpers/Movement.hpp"
#include "Game/Core/Simulation/CommandBuffer.hpp"
#include "Game/Core/Tools/Random.hpp"
#include "Game/Core/Registrator.hpp"
#include "Game/Features/Components/ActiveComponent.hpp"
#include "Game/Features/Components/PoisonEffectComponent.hpp"
#include "Game/Features/Components/UnitIdComponent.hpp"
#include "Game/Features/Components/HealthComponent.hpp"
#include "Game/Features/Components/PositionComponent.hpp"
#include "IO/Commands/SpawnHunter.hpp"
#include "IO/Events/UnitAbilityUsed.hpp"

namespace sw::game::behaviour
{
	namespace
	{
		void tick(
				const event::ActionTick&,
				const core::EventContext& context,
				core::EntityId entityId,
				const HunterBehaviour& behaviour,
				component::PositionComponent& position,
				component::ActiveComponent& active,
				const component::UnitIdComponent& userId)
		{
			active.active = false;

			if (const auto target = findTargetForMeleeAttack(context, entityId, position.position))
			{
				attack(context, entityId, *target, DamageUnit<DamageKind::Physical>{behaviour.strength});
				active.active = true;
				return;
			}

			if (const auto target = findTargetForDistanceAttack(context, entityId, position.position, 2, behaviour.range))
			{
				if (core::checkChance(context.random, behaviour.chance))
				{
					context.commands.addComponent(
							*target, component::PoisonEffectComponent::fromTotalDamage(entityId, behaviour.poison, 5));
					context.eventSystem.event(
							context.tick,
							io::UnitAbilityUsed{
									.abilityUnitId = userId.id.get_raw(), .abilityName = "poison_arrow"});
				}
				else
				{
					attack(context, entityId, *target, DamageUnit<DamageKind::Physical>{behaviour.agility});
				}
				active.active = true;
				return;
			}

			if (moveUnit(context, entityId, position))
			{
				active.active = true;
			}
		}
	}

	void registerHunter(core::Registrator& registrator)
	{
		registrator.addSpawner(
				+[](const io::SpawnHunter& command, core::CommandBuffer& buffer)
				{
					buffer.spawnEntity(
							HunterBehaviour{
									.agility = static_cast<std::int32_t>(command.agility),
									.strength = static_cast<std::int32_t>(command.strength),
									.range = static_cast<std::int32_t>(command.range),
									.chance = static_cast<std::int32_t>(command.chance),
									.poison = static_cast<std::int32_t>(command.poison)
							},
							component::PositionComponent{.position = {.x = command.x, .y = command.y}},
							component::UnitIdComponent{
									.id = UnitId{command.unitId},
									.type = "hunter"
							},
							component::HealthComponent{.health = static_cast<int>(command.hp)},
							component::ActiveComponent{true}
							);
				}
				);

		registrator.handlers().add(&tick);
	}
}
