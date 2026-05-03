#include "DistanceAttack.hpp"
#include "Attack.hpp"
#include "Game/Core/Simulation/Map.hpp"
#include "Game/Core/Event/EventHandlerSystem.hpp"

namespace sw::game::behaviour
{
	std::optional<core::EntityId> findTargetForDistanceAttack(
			const core::EventContext& context,
			core::EntityId entityId,
			const core::Position& position,
			std::uint32_t minDistance,
			std::uint32_t maxDistance)
	{
		return context.map.findFirstEntity(
				[&](core::EntityId id, const core::Position& target_pos)
				{
					const auto dist = core::distance(position, target_pos);
					return id != entityId && dist >= minDistance && dist <= maxDistance;
				});
	}
}
