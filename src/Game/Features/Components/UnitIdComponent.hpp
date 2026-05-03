#pragma once
#include "Game/Core/Tools/TypedId.hpp"

#include <string_view>

namespace sw::game
{
	struct Unit;
	using UnitId = core::TypedId<Unit, std::uint32_t>;
}

namespace sw::game::component
{
	struct UnitIdComponent
	{
		UnitId id;
		std::string_view type;
	};
}
