#pragma once

#include "Map.hpp"
#include "Unit.hpp"

#include <vector>

namespace sw
{
	class EventSystem;

	namespace io
	{
		struct March;
		struct CreateMap;
		struct SpawnSwordsman;
	}
}

namespace sw::game
{
	class World
	{
	public:
		explicit World(EventSystem& events);
		~World() = default;

		void createMap(uint32_t width, uint32_t height);
		void spawnUnit(
				uint32_t unitId, Position position, uint32_t hp, uint32_t strength, uint32_t chance, uint32_t rending);
		void march(uint32_t unitId, Position targetPosition);

		[[nodiscard]]
		bool isGameOver() const;
		void step();

	private:
		Unit* getUnitById(uint32_t unitId);

		Map _map;
		EventSystem& _events;
		std::vector<Unit> _units;
		uint32_t _tick;
	};
}
