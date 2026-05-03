#pragma once

#include "Game/Core/Tools/TypedId.hpp"

namespace sw::core
{
	struct Entity;

	using EntityId = TypedId<Entity, uint32_t>;
}
