#include "Game/World.hpp"
#include "IO/Commands/CreateMap.hpp"
#include "IO/Commands/March.hpp"
#include "IO/Commands/SpawnHunter.hpp"
#include "IO/Commands/SpawnSwordsman.hpp"
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

	EventSystem events;
	game::World world(events);

	io::CommandParser parser;
	parser.add<io::CreateMap>([&world](auto command) { world.createMap(command.width, command.height); });
	parser.add<io::SpawnSwordsman>(
			[&world](auto command)
			{
				world.spawnUnit(
						command.unitId,
						{command.x, command.y},
						command.hp,
						command.strength,
						command.chance,
						command.rending);
			});
	parser.add<io::SpawnHunter>(
			[](auto command)
			{
				//TODO
			});
	parser.add<io::March>([&world](auto command) { world.march(command.unitId, {command.targetX, command.targetY}); });

	parser.parse(file);

	while (!world.isGameOver())
	{
		world.step();
	}

	return 0;
}
