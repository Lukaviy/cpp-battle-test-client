#pragma once
#include "Game/Core/Event/EventHandlerSystem.hpp"
#include "Game/Core/Component/ComponentsContainer.hpp"
#include "Game/Core/Entity.hpp"
#include "Game/Core/Simulation/Map.hpp"
#include "Game/Features/Components/UnitIdComponent.hpp"

#include <random>

namespace sw::core
{
	class CommandBuffer;

	class World
	{
		friend CommandBuffer;

	public:
		World(std::mt19937 random);
		void round(EventSystem& eventSystem);

		void createMap(uint32_t width, uint32_t height);

		bool isGameOver() const;

		void setEventRegistry(EventHandlerSystem&& eventRegistry);

		void march(Position position, game::UnitId entityId, EventSystem& eventSystem);

		std::uint64_t currentTick() const;

		std::mt19937& getRandomEngine();

		std::optional<EntityId> getEntityIdByUnitId(game::UnitId unitId);
	private:
		EntityId generateNewEntityId();
		void registerSpawnedEntity(EntityId entityId);

		EventHandlerSystem _eventRegistry;
		ComponentsContainer _componentsContainer;

		struct EntityEntry
		{
			EntityId id;
			bool alive{true};
		};

		std::vector<EntityEntry> _entities;
		std::unordered_map<game::UnitId, EntityId> _unitIdToEntityId;
		Map _map;
		EntityId::underlying_type _lastEntityId{};
		uint64_t _tick{};
		std::mt19937 _random;
	};
}
