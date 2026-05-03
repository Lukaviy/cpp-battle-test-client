#pragma once
#include "Game/Core/Simulation/CommandBuffer.hpp"
#include "Game/Core/Event/EventHandlerSystem.hpp"
#include "IO/System/CommandParser.hpp"

namespace sw::core
{
	class Registrator
	{
	public:
		Registrator(
				io::CommandParser& commandParser,
				World& world,
				EventSystem& eventSystem,
				EventHandlerRegistry& eventHandlerRegistry)
			: _commandParser(commandParser),
			  _world(world),
			  _eventSystem(eventSystem),
			  _eventHandlerRegistry(eventHandlerRegistry)
		{
		}

		template <class T>
		void addSpawner(void (*spawner)(const T& command, CommandBuffer& command_buffer))
		{
			_commandParser.add<std::remove_cvref_t<T>>(
					[spawner, world{&_world}, event_system{&_eventSystem}](const T& command)
					{
						CommandBuffer commandBuffer;
						spawner(command, commandBuffer);
						commandBuffer.performCommands(
								{.world = *world, .eventSystem = *event_system, .tick = world->currentTick()});
					});
		}

		EventHandlerRegistry& handlers() const
		{
			return _eventHandlerRegistry;
		}

	private:
		io::CommandParser& _commandParser;
		World& _world;
		EventSystem& _eventSystem;
		EventHandlerRegistry& _eventHandlerRegistry;
	};
}
