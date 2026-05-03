#include "CommandBuffer.hpp"

namespace sw::core
{
	void CommandBuffer::removeEntity(EntityId entityId)
	{
		_commands.emplace_back(
				[entityId](const CommandContext& context) mutable
				{
					context.world._map.remove(entityId);

					const auto it = std::ranges::find(context.world._entities, entityId, &World::EntityEntry::id);
					if (it != context.world._entities.end())
					{
						it->alive = false;
					}

					{
						CommandBuffer command_buffer;
						context.world._eventRegistry.emit(
								game::event::Die{},
								EventContext{
										.commands = command_buffer,
										.components = context.world._componentsContainer,
										.eventSystem = context.eventSystem,
										.eventRegistry = context.world._eventRegistry,
										.map = context.world._map,
										.random = context.world._random,
										.tick = context.world._tick,
								}, entityId);

						CommandContext command_context{
								.world = context.world,
								.eventSystem = context.eventSystem,
								.tick = context.tick,
						};

						command_buffer.performCommands(command_context);
					}
				}
				);
	}

	void CommandBuffer::performCommands(const CommandContext& context)
	{
		for (auto& command : _commands)
		{
			command(context);
		}
		_commands.clear();
	}
}
