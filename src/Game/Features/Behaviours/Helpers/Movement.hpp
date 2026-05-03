#pragma once
#include "Game/Core/Entity.hpp"

namespace sw::game::component
{
	struct PositionComponent;
}

namespace sw::core
{
	struct EventContext;
}

namespace sw::game::behaviour
{
	bool moveUnit(
			const core::EventContext& context,
			core::EntityId entityId,
			component::PositionComponent& position_component);
}
