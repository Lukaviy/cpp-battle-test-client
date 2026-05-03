#pragma once
#include <cstdint>
#include "Game/Core/Tools/TypedId.hpp"

namespace sw::core
{
	struct Component;

	using ComponentId = TypedId<Component, std::uint32_t>;
}
