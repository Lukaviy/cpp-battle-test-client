#include "Map.hpp"

#include <algorithm>
#include <ranges>

namespace sw::core
{
	Map::Map(const uint32_t width, const uint32_t height) :
		_width(width),
		_height(height)
	{
	}

	bool Map::isPositionFree(const Position position) const
	{
		if (position.x >= _width || position.y >= _height)
		{
			return false;
		}
		return !std::ranges::any_of(
				_positionByUnitId,
				[&position](const auto& unitPositionPair)
				{
					return unitPositionPair.second == position;
				});
	}

	bool Map::isPositionValid(Position position) const
	{
		return position.x < _width && position.y < _height;
	}

	bool Map::tryGetNextPosition(
			const Position currentPosition,
			const Position targetPosition,
			Position& outPosition) const
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

	void Map::move(const EntityId entityId, const Position position)
	{
		_positionByUnitId[entityId] = position;
	}

	void Map::remove(const EntityId entityId)
	{
		_positionByUnitId.erase(entityId);
	}
}
