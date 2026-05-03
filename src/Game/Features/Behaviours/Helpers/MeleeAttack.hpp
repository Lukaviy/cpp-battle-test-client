#pragma once
#include "Game/Core/Entity.hpp"

#include <optional>

namespace sw::core
{
	struct Position;
	struct EventContext;
}

namespace sw::game::behaviour
{
	std::optional<core::EntityId> findTargetForMeleeAttack(
			const core::EventContext& context,
			core::EntityId entityId,
			const core::Position& position);
}
