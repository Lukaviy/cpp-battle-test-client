#include "Map.hpp"

#include <algorithm>
#include <ranges>

namespace sw::game
{
	Map::Map(const uint32_t width, const uint32_t height) :
			_width(width),
			_height(height)
	{}

	bool Map::isPositionFree(const Position position) const
	{
		if (position.x >= _width || position.y >= _height)
		{
			return false;
		}
		return !std::ranges::any_of(
				_positionByUnitId,
				[&position](const auto& unitPositionPair) { return unitPositionPair.second == position; });
	}

	void Map::findTargets(
			const uint32_t currentUnitId, const Position currentPosition, std::vector<uint32_t>& outUnitIds) const
	{
		for (auto [unitId, unitPosition] : _positionByUnitId)
		{
			if (currentUnitId == unitId)
			{
				continue;
			}
			const int64_t dx = abs(static_cast<int64_t>(currentPosition.x) - static_cast<int64_t>(unitPosition.x));
			const int64_t dy = abs(static_cast<int64_t>(currentPosition.y) - static_cast<int64_t>(unitPosition.y));
			if (dx <= 1 && dy <= 1)
			{
				outUnitIds.push_back(unitId);
			}
		}
	}

	bool Map::tryGetNextPosition(
			const Position currentPosition, const Position targetPosition, Position& outPosition) const
	{
		Position nextPosition = currentPosition;
		if (targetPosition.x < currentPosition.x)
		{
			nextPosition.x -= 1;
		}
		else if (targetPosition.x > currentPosition.x)
		{
			nextPosition.x += 1;
		}
		if (targetPosition.y < currentPosition.y)
		{
			nextPosition.y -= 1;
		}
		else if (targetPosition.y > currentPosition.y)
		{
			nextPosition.y += 1;
		}
		if (isPositionFree(nextPosition))
		{
			outPosition = nextPosition;
			return true;
		}
		return false;
	}

	void Map::move(const uint32_t unitId, const Position position)
	{
		_positionByUnitId[unitId] = position;
	}

	void Map::remove(const uint32_t unitId)
	{
		_positionByUnitId.erase(unitId);
	}
}
