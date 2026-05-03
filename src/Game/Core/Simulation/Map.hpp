#pragma once

#include "Game/Core/Position.hpp"
#include "Game/Core/Entity.hpp"

#include <map>
#include <optional>
#include <vector>

namespace sw::core
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

		[[nodiscard]]
		bool isPositionValid(Position position) const;

		bool tryGetNextPosition(Position currentPosition, Position targetPosition, Position& outPosition) const;

		void move(EntityId entityId, Position position);
		void remove(EntityId entityId);

		template <class F>
			requires requires(F f, EntityId id, Position pos) {
				{ f(id, pos) } -> std::convertible_to<bool>;
			}
		std::optional<EntityId> findFirstEntity(F&& predicate) const
		{
			for (const auto& [id, pos] : _positionByUnitId)
			{
				if (predicate(id, pos))
				{
					return id;
				}
			}

			return std::nullopt;
		}

	private:
		uint32_t _width{0};
		uint32_t _height{0};

		std::map<EntityId, Position> _positionByUnitId;
	};
}
