#include "World.hpp"

#include "CommandBuffer.hpp"
#include "Game/Features/Events/BasicEvents.hpp"
#include "Game/Features/Components/ActiveComponent.hpp"
#include "Game/Features/Components/MarchTargetComponent.hpp"

#include <format>

namespace sw::core
{
	World::World(std::mt19937 random) : _map(0, 0), _random(random)
	{
	}

	void World::round(EventSystem& eventSystem)
	{
		const auto emitTick = [&]<typename T>(T event, const EntityEntry& entry)
		{
			if (!entry.alive)
			{
				return;
			}

			CommandBuffer command_buffer;
			CommandContext commandContext{*this, eventSystem, _tick};
			_eventRegistry.emit(
					event,
					EventContext{
							.commands = command_buffer,
							.components = _componentsContainer,
							.eventSystem = eventSystem,
							.eventRegistry = _eventRegistry,
							.map = _map,
							.random = _random,
							.tick = _tick},
					entry.id);
			command_buffer.performCommands(commandContext);
		};

		const auto entityCount = _entities.size();

		for (std::size_t i = 0; i < entityCount; ++i)
		{
			emitTick(game::event::RoundStartTick{}, _entities[i]);
		}

		for (std::size_t i = 0; i < entityCount; ++i)
		{
			for (std::size_t j = 0; j < entityCount; ++j)
			{
				emitTick(
						game::event::TurnStartTick{_entities[i].id},
						_entities[j]);
			}
			emitTick(game::event::ActionTick{}, _entities[i]);
			for (std::size_t j = 0; j < entityCount; ++j)
			{
				emitTick(
						game::event::TurnEndTick{_entities[i].id},
						_entities[j]);
			}
		}

		for (std::size_t i = 0; i < entityCount; ++i)
		{
			emitTick(game::event::RoundEndTick{}, _entities[i]);
		}

		std::erase_if(
				_entities,
				[&](const EntityEntry& entry)
				{
					if (!entry.alive)
					{
						if (auto* const userEntityId
								= _componentsContainer.getComponent<game::component::UnitIdComponent>(entry.id))
						{
							_unitIdToEntityId.erase(userEntityId->id);
						}

						_componentsContainer.removeComponents(entry.id);

						return true;
					}
					return false;
				});

		++_tick;
	}

	void World::createMap(uint32_t width, uint32_t height)
	{
		_map = Map(width, height);
	}

	bool World::isGameOver() const
	{
		return std::ranges::none_of(
				_componentsContainer.getComponents<game::component::ActiveComponent>(),
				&game::component::ActiveComponent::active);
	}

	void World::setEventRegistry(EventHandlerSystem&& eventRegistry)
	{
		_eventRegistry = std::move(eventRegistry);
	}

	void World::march(Position position, game::UnitId entityId, EventSystem& eventSystem)
	{
		const auto it = _unitIdToEntityId.find(entityId);
		if (it == _unitIdToEntityId.end())
		{
			return;
		}
		CommandBuffer commandBuffer;
		commandBuffer.addComponent(it->second, game::component::MarchTargetComponent{.position = position});
		commandBuffer.performCommands(CommandContext{*this, eventSystem, _tick});
	}

	std::uint64_t World::currentTick() const
	{
		return _tick;
	}

	std::mt19937& World::getRandomEngine()
	{
		return _random;
	}

	std::optional<EntityId> World::getEntityIdByUnitId(game::UnitId unitId)
	{
		const auto it = _unitIdToEntityId.find(unitId);
		if (it == _unitIdToEntityId.end())
		{
			return std::nullopt;
		}
		return it->second;
	}

	EntityId World::generateNewEntityId()
	{
		return EntityId{++_lastEntityId};
	}

	void World::registerSpawnedEntity(EntityId entityId)
	{
		_entities.push_back(EntityEntry{.id = entityId, .alive = true});

		if (const auto* const unitId = _componentsContainer.getComponent<game::component::UnitIdComponent>(entityId))
		{
			const auto inserted = _unitIdToEntityId.insert({unitId->id, entityId}).second;
			if (!inserted)
			{
				throw std::runtime_error{std::format("Error: Duplicate unit ID: {}", unitId->id.get_raw())};
			}
		}

		if (const auto* const position
				= _componentsContainer.getComponent<game::component::PositionComponent>(entityId))
		{
			if (!_map.isPositionValid(position->position))
			{
				throw std::runtime_error{std::format(
						"Error: Invalid position for entity {}: ({}, {})",
						entityId.get_raw(),
						position->position.x,
						position->position.y)};
			}
			_map.move(entityId, position->position);
		}
	}
}
