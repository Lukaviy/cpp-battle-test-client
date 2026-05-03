#pragma once
#include "World.hpp"
#include "Game/Features/Components/PositionComponent.hpp"
#include "Game/Features/Events/BasicEvents.hpp"
#include "IO/System/EventSystem.hpp"
#include "IO/Events/UnitSpawned.hpp"

#include <functional>
#include <vector>

namespace sw::core
{
	class World;

	struct CommandContext
	{
		World& world;
		EventSystem& eventSystem;
		std::uint64_t tick{};
	};

	class CommandBuffer
	{
	public:
		template <class... Args>
		void spawnEntity(Args&&... args)
		{
			_commands.emplace_back(
					[...args = std::forward<Args>(args)](const CommandContext& context) mutable
					{
						const auto newEntityId = context.world.generateNewEntityId();
						(context.world._componentsContainer.addComponent(newEntityId, std::move(args)), ...);
						context.world.registerSpawnedEntity(newEntityId);

						CommandBuffer command_buffer;
						context.world._eventRegistry.emit(
								game::event::Spawn{},
								EventContext{
										.commands = command_buffer,
										.components = context.world._componentsContainer,
										.eventSystem = context.eventSystem,
										.eventRegistry = context.world._eventRegistry,
										.map = context.world._map,
										.random = context.world._random,
										.tick = context.tick,
								},
								newEntityId);
						command_buffer.performCommands(context);
					});
		}

		void removeEntity(EntityId entityId);

		template <class T>
		void addComponent(EntityId entityId, T component)
		{
			_commands.emplace_back(
					[entityId, component](const CommandContext& context) mutable
					{
						CommandBuffer command_buffer;
						if (auto* const existing_component = context.world._componentsContainer.getComponent<T>(
								entityId))
						{
							context.world._eventRegistry.emit(
									game::event::AddExisting<T>(component),
									EventContext{
											.commands = command_buffer,
											.components = context.world._componentsContainer,
											.eventSystem = context.eventSystem,
											.eventRegistry = context.world._eventRegistry,
											.map = context.world._map,
											.random = context.world._random,
											.tick = context.tick,
									},
									entityId);
							command_buffer.performCommands(context);

							*existing_component = component;
						}
						else
						{
							context.world._componentsContainer.addComponent(entityId, component);
							context.world._eventRegistry.emit(
									game::event::Add<T>(),
									EventContext{
											.commands = command_buffer,
											.components = context.world._componentsContainer,
											.eventSystem = context.eventSystem,
											.eventRegistry = context.world._eventRegistry,
											.map = context.world._map,
											.random = context.world._random,
											.tick = context.tick,
									},
									entityId);
							command_buffer.performCommands(context);
						}
					});
		}

		template <class T>
		void removeComponent(EntityId entityId)
		{
			_commands.emplace_back(
					[entityId](const CommandContext& context) mutable
					{
						CommandBuffer command_buffer;
						context.world._eventRegistry.emit(
								game::event::Remove<T>(),
								EventContext{
										.commands = command_buffer,
										.components = context.world._componentsContainer,
										.eventSystem = context.eventSystem,
										.eventRegistry = context.world._eventRegistry,
										.map = context.world._map,
										.random = context.world._random,
										.tick = context.tick,
								},
								entityId);
						command_buffer.performCommands(context);
						context.world._componentsContainer.removeComponent<T>(entityId);
					});
		}

		void performCommands(const CommandContext& context);

	private:
		std::vector<std::function<void(const CommandContext&)>> _commands;
	};
}
