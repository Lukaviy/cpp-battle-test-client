#include "Game/Core/Simulation/World.hpp"
#include "Game/Core/Simulation/CommandBuffer.hpp"
#include "Game/Core/Registrator.hpp"
#include "Game/Features/Registry.hpp"
#include "IO/Commands/CreateMap.hpp"
#include "IO/Commands/March.hpp"
#include "IO/System/CommandParser.hpp"
#include "IO/System/EventSystem.hpp"

#include <fstream>
#include <thread>

int main(int argc, char** argv)
{
	using namespace sw;

	if (argc != 2)
	{
		throw std::runtime_error("Error: No file specified in command line argument");
	}

	std::ifstream file(argv[1]);
	if (!file)
	{
		throw std::runtime_error("Error: File not found - " + std::string(argv[1]));
	}

	EventSystem events{std::cout};

	core::EventHandlerRegistry eventRegistry;
	std::mt19937 random;
	core::World world{random};
	io::CommandParser parser;

	core::Registrator registrator{parser, world, events, eventRegistry};
	game::registerAll(registrator);

	world.setEventRegistry(std::move(eventRegistry).createEventHandlerSystem());

	parser.add<io::CreateMap>(
			[&world](const io::CreateMap& command)
			{
				world.createMap(command.width, command.height);
			});

	parser.add<io::March>(
			[&world, &events](const io::March& command)
			{
				world.march({command.targetX, command.targetY}, game::UnitId{command.unitId}, events);
			});

	parser.parse(file);

	while (!world.isGameOver())
	{
		world.round(events);
	}

	return 0;
}
