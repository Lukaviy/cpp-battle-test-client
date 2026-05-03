#pragma once

#include "Attack.hpp"
#include "Game/Core/Entity.hpp"

#include <optional>

namespace sw::core
{
	struct Position;
	struct EventContext;
}

namespace sw::game::behaviour
{
	std::optional<core::EntityId> findTargetForDistanceAttack(
			const core::EventContext& context,
			core::EntityId entityId,
			const core::Position& position,
			std::uint32_t minDistance,
			std::uint32_t maxDistance);
}
