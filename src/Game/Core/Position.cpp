#include "Position.hpp"

#include <algorithm>

namespace sw::core
{
	std::uint32_t distance(const Position& a, const Position& b)
	{
		const auto dx = static_cast<std::int32_t>(b.x) - static_cast<std::int32_t>(a.x);
		const auto dy = static_cast<std::int32_t>(b.y) - static_cast<std::int32_t>(a.y);

		return static_cast<std::uint32_t>(std::max(std::abs(dx), std::abs(dy)));
	}
}
