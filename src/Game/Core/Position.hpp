#pragma once

#include <cstdint>
#include <compare>

namespace sw::core
{
	struct Position
	{
		uint32_t x{0};
		uint32_t y{0};

		auto operator<=>(const Position&) const = default;
	};

	std::uint32_t distance(const Position& a, const Position& b);
}
