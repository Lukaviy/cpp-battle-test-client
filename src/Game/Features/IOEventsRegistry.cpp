#include "IOEventsRegistry.hpp"

#include "Game/Core/Registrator.hpp"

#include "Components/UnitIdComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/MarchTargetComponent.hpp"
#include "Components/PositionComponent.hpp"

#include "Events/BasicEvents.hpp"
#include "Events/DamageEvents.hpp"

#include "IO/Events/MarchStarted.hpp"
#include "IO/Events/MarchEnded.hpp"
#include "IO/Events/UnitAttacked.hpp"
#include "IO/Events/UnitSpawned.hpp"
#include "IO/Events/UnitDied.hpp"
#include "IO/System/EventSystem.hpp"

namespace sw::game
{
	void registerIOEventHandlers(core::Registrator& registrator)
	{
		registrator.handlers().add(
				+[](
				const event::Add<component::MarchTargetComponent>&,
				const core::EventContext& context,
				core::EntityId,
				const component::MarchTargetComponent& marchTarget,
				const component::UnitIdComponent& unitId,
				const component::PositionComponent& position)
				{
					context.eventSystem.event(
							context.tick,
							io::MarchStarted{
									.unitId = unitId.id.get_raw(),
									.x = position.position.x,
									.y = position.position.y,
									.targetX = marchTarget.position.x,
									.targetY = marchTarget.position.y
							});
				});

		registrator.handlers().add(
				+[](const event::Remove<component::MarchTargetComponent>&,
				const core::EventContext& context,
				core::EntityId,
				const component::MarchTargetComponent&,
				const component::UnitIdComponent& unitId,
				const component::PositionComponent& position)
				{
					context.eventSystem.event(
							context.tick,
							io::MarchEnded{
									.unitId = unitId.id.get_raw(),
									.x = position.position.x,
									.y = position.position.y});
				});

		registrator.handlers().add(
				+[](
				const event::Spawn&,
				const core::EventContext& context,
				core::EntityId,
				const component::UnitIdComponent& unitId,
				const component::PositionComponent& position)
				{
					context.eventSystem.event(
							context.tick,
							io::UnitSpawned{
									.unitId = unitId.id.get_raw(),
									.unitType = std::string{unitId.type},
									.x = position.position.x,
									.y = position.position.y});
				});

		registrator.handlers().add(
				+[](
				const event::Die&,
				const core::EventContext& context,
				core::EntityId,
				const component::UnitIdComponent& user_id)
				{
					context.eventSystem.event(context.tick, io::UnitDied{user_id.id.get_raw()});
				});

		registrator.handlers().add(
				+[](
				const event::ReceiveDamageEvent& event,
				const core::EventContext& context,
				core::EntityId,
				const component::UnitIdComponent& targetUserEntityId,
				const component::HealthComponent& health)
				{
					const auto sourceUserEntityId
							= context.components.getComponent<component::UnitIdComponent>(
									event.sourceEntity);
					if (!sourceUserEntityId)
					{
						return;
					}
					context.eventSystem.event(
							context.tick,
							io::UnitAttacked{
									.attackerUnitId = sourceUserEntityId->id.get_raw(),
									.targetUnitId = targetUserEntityId.id.get_raw(),
									.damage = static_cast<uint32_t>(totalDamage(event.damage)),
									.targetHp = static_cast<uint32_t>(health.health)});
				});
	}
}
