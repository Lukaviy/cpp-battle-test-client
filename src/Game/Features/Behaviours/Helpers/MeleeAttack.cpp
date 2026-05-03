#include "MeleeAttack.hpp"
#include "Attack.hpp"
#include "Game/Core/Simulation/Map.hpp"
#include "Game/Core/Event/EventHandlerSystem.hpp"

namespace sw::game::behaviour
{
	std::optional<core::EntityId> findTargetForMeleeAttack(
			const core::EventContext& context,
			core::EntityId entityId,
			const core::Position& position)
	{
		return context.map.findFirstEntity(
				[&](core::EntityId id, const core::Position& target_pos)
				{
					return id != entityId && core::distance(position, target_pos) <= 1;
				});
	}
}
