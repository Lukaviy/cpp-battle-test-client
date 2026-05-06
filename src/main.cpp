#include "Core/Random.hpp"
#include "Core/World.hpp"
#include "Features/Common/MarchHandler.hpp"
#include "Features/Hunter/Hunter.hpp"
#include "Features/Swordsman/Swordsman.hpp"
#include "IO/Commands/CreateMap.hpp"
#include "IO/Commands/March.hpp"
#include "IO/Commands/SpawnHunter.hpp"
#include "IO/Commands/SpawnSwordsman.hpp"
#include "IO/Events/MapCreated.hpp"
#include "IO/System/CommandParser.hpp"
#include "IO/System/EventSystem.hpp"

#include <fstream>
#include <memory>

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
	core::World world(events, std::make_unique<core::DefaultRandom>());

	io::CommandParser parser;
	parser.add<io::CreateMap>(
			[&world](auto command)
			{
				world.createMap(command.width, command.height);
				world.events().event(world.tick(), io::MapCreated{command.width, command.height});
			});
	parser.add<io::SpawnSwordsman>([&world](auto command) { features::swordsman::spawn(world, command); });
	parser.add<io::SpawnHunter>([&world](auto command) { features::hunter::spawn(world, command); });
	parser.add<io::March>([&world](auto command) { features::handleMarch(world, command); });

	parser.parse(file);

	while (!world.isQuiescent())
	{
		if (!world.step())
		{
			break;
		}
	}

	return 0;
}
