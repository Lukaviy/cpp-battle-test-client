#pragma once
#include "Game/Core/Tools/TypedId.hpp"

#include <cstdint>

namespace sw::core
{
	struct Event;

	using EventId = TypedId<Event, std::uint32_t>;
}
