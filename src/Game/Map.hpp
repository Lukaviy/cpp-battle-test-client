#pragma once

#include "Position.hpp"

#include <map>
#include <vector>

namespace sw::game
{
	class Map
	{
	public:
		Map(uint32_t width, uint32_t height);
		~Map() = default;

		[[nodiscard]]
		bool isValid() const
		{
			return _width > 0 && _height > 0;
		}

		[[nodiscard]]
		bool isPositionFree(Position position) const;

		void findTargets(uint32_t currentUnitId, Position currentPosition, std::vector<uint32_t>& outUnitIds) const;
		bool tryGetNextPosition(Position currentPosition, Position targetPosition, Position& outPosition) const;

		void move(uint32_t unitId, Position position);
		void remove(uint32_t unitId);

	private:
		uint32_t _width{0};
		uint32_t _height{0};

		std::map<uint32_t, Position> _positionByUnitId;
	};
}
