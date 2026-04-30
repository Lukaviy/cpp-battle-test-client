#pragma once

#include <cstdint>

namespace sw::game
{
	struct Position
	{
		Position() = default;

		Position(const uint32_t x, const uint32_t y) :
				x(x),
				y(y)
		{}

		Position(const Position& value) = default;
		Position(Position&& value) = default;
		~Position() = default;

		Position& operator=(const Position& value) = default;
		Position& operator=(Position&& value) = default;

		uint32_t x{0};
		uint32_t y{0};
	};

	constexpr bool operator==(const Position& left, const Position& right) noexcept
	{
		return left.x == right.x && left.y == right.y;
	}

	constexpr bool operator!=(const Position& left, const Position& right) noexcept
	{
		return left.x != right.x || left.y != right.y;
	}
}
