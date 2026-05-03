#include "SwordsmanBehaviour.hpp"

#include "Helpers/Attack.hpp"
#include "Helpers/MeleeAttack.hpp"
#include "Helpers/Movement.hpp"
#include "Game/Core/Simulation/CommandBuffer.hpp"
#include "Game/Core/Event/EventHandlerSystem.hpp"
#include "Game/Core/Tools/Random.hpp"
#include "Game/Core/Registrator.hpp"
#include "Game/Features/Events/BasicEvents.hpp"
#include "Game/Features/Components/ActiveComponent.hpp"
#include "Game/Features/Components/PositionComponent.hpp"
#include "Game/Features/Components/UnitIdComponent.hpp"
#include "Game/Features/Components/HealthComponent.hpp"
#include "Game/Features/Components/MarchTargetComponent.hpp"
#include "Game/Features/Components/RendingEffectComponent.hpp"
#include "IO/Commands/SpawnSwordsman.hpp"
#include "IO/Events/UnitAbilityUsed.hpp"

namespace sw::game::behaviour
{
	namespace
	{
		void tick(
				const event::ActionTick&,
				const core::EventContext& context,
				core::EntityId entityId,
				const SwordsmanBehaviour& behaviour,
				component::PositionComponent& position,
				component::ActiveComponent& active,
				const component::UnitIdComponent& userId
				)
		{
			active.active = false;

			if (const auto target_opt = findTargetForMeleeAttack(context, entityId, position.position))
			{
				const auto target = *target_opt;

				if (core::checkChance(context.random, behaviour.rendingAbilityChance))
				{
					context.commands.addComponent(target, component::RendingEffectComponent{});
					context.eventSystem.event(
							context.tick,
							io::UnitAbilityUsed{.abilityUnitId = userId.id.get_raw(), .abilityName = "rending"});

					attack(context, entityId, target, DamageUnit<DamageKind::Physical>{behaviour.rending});
				}
				else
				{
					attack(context, entityId, target, DamageUnit<DamageKind::Physical>{behaviour.strength});
				}

				active.active = true;
			}
			else if (moveUnit(context, entityId, position))
			{
				active.active = true;
			}
		}
	}

	void registerSwordsman(core::Registrator& registrator)
	{
		registrator.addSpawner(
				+[](const io::SpawnSwordsman& command, core::CommandBuffer& buffer)
				{
					buffer.spawnEntity(
							SwordsmanBehaviour{.strength = static_cast<int>(command.strength),
							                   .rending = static_cast<int>(command.rending),
							                   .rendingAbilityChance = static_cast<int>(command.chance)},
							component::PositionComponent{.position = {.x = command.x, .y = command.y}},
							component::UnitIdComponent{
									.id = UnitId{command.unitId},
									.type = "swordsman"
							},
							component::HealthComponent{.health = static_cast<int>(command.hp)},
							component::ActiveComponent{true});
				});

		registrator.handlers().add(&tick);
	}
}
