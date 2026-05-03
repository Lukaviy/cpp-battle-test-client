#include "Movement.hpp"

#include "Game/Core/Simulation/Map.hpp"
#include "Game/Core/Event/EventHandlerSystem.hpp"
#include "Game/Core/Simulation/CommandBuffer.hpp"
#include "Game/Features/Components/MarchTargetComponent.hpp"
#include "Game/Features/Components/PositionComponent.hpp"
#include "IO/Events/UnitMoved.hpp"
#include "IO/System/EventSystem.hpp"

namespace sw::game::behaviour
{
	namespace
	{
		bool basicMove(
				const core::EventContext& context,
				core::EntityId entityId,
				component::PositionComponent& position_component,
				component::MarchTargetComponent& move_target)
		{
			core::Position next_position;
			if (context.map.tryGetNextPosition(position_component.position, move_target.position, next_position))
			{
				position_component.position = next_position;
				context.map.move(entityId, next_position);
				context.eventSystem.event(
						context.tick, io::UnitMoved{entityId.get_raw(), next_position.x, next_position.y});
				if (move_target.position == next_position)
				{
					context.commands.removeComponent<component::MarchTargetComponent>(entityId);
				}
				return true;
			}
			return false;
		}
	}

	bool moveUnit(const core::EventContext& context, core::EntityId entityId, component::PositionComponent& position_component)
	{
		if (auto* const move_target = context.components.getComponent<
			component::MarchTargetComponent>(entityId))
		{
			return basicMove(context, entityId, position_component, *move_target);
		}
		return false;
	}
}
