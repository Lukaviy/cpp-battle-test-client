#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include "test_utils.hpp"
#include "IO/Commands/CreateMap.hpp"
#include "IO/Commands/SpawnHunter.hpp"
#include "IO/Commands/SpawnSwordsman.hpp"
#include "IO/Events/UnitAttacked.hpp"

TEST_CASE("Poison effect deals damage over time", "[effects][poison][combat]")
{
	// Use fixed seed for reproducibility
	test_utils::TestEnvironment env(12345);

	// Create map
	env.executeCommand(sw::io::CreateMap{.width = 10, .height = 10});

	// Spawn hunter with 100% poison chance
	env.executeCommand(sw::io::SpawnHunter{
		.unitId = 1,
		.x = 0,
		.y = 0,
		.hp = 100,
		.agility = 10,
		.strength = 5,
		.range = 5,
		.chance = 1000,
		.poison = 50
	});
	// Spawn target swordsman at range
	env.executeCommand(sw::io::SpawnSwordsman{
		.unitId = 2,
		.x = 5,
		.y = 0,
		.hp = 100,
		.strength = 10,
		.chance = 0,
		.rending = 0
	});
	env.executeCommand(sw::io::March{
		.unitId = 2,
		.targetX = 6,
		.targetY = 7
	});

	env.clearOutput();

	// Run rounds - hunter should use poison attack
	int totalDamage = 0;
	int attackCount = 0;

	for (int round = 0; round < 10; ++round)
	{
		env.world.round(env.events);

		// Parse attack events from this round
		env.resetOutput();
		while (auto eventOpt = env.parseEvent<sw::io::UnitAttacked>())
		{
			const auto& [tick, attack] = *eventOpt;
			if (attack.targetUnitId == 2)
			{
				attackCount++;
				totalDamage += attack.damage;
			}
		}

		env.clearOutput();
	}

	// Poison should deal 50 damage over 5 turns (10 damage per turn)
	// Plus initial attack damage from hunter
	REQUIRE(attackCount == 5);
	REQUIRE(totalDamage == 50);
}

TEST_CASE("Multiple poison effects stack correctly", "[effects][poison][stacking][combat]")
{
	// Use fixed seed for reproducibility
	test_utils::TestEnvironment env(54321);

	// Create map
	env.executeCommand(sw::io::CreateMap{.width = 10, .height = 10});

	// Spawn first hunter with poison: 30 total damage over 5 turns (6 per turn)
	env.executeCommand(sw::io::SpawnHunter{
		.unitId = 1,
		.x = 0,
		.y = 0,
		.hp = 100,
		.agility = 10,
		.strength = 5,
		.range = 5,
		.chance = 1000,
		.poison = 30
	});

	// Spawn second hunter with poison: 40 total damage over 5 turns (8 per turn)
	env.executeCommand(sw::io::SpawnHunter{
		.unitId = 2,
		.x = 9,
		.y = 0,
		.hp = 100,
		.agility = 10,
		.strength = 5,
		.range = 5,
		.chance = 1000,
		.poison = 40
	});

	// Spawn target swordsman at range with high HP
	env.executeCommand(sw::io::SpawnSwordsman{
		.unitId = 3,
		.x = 5,
		.y = 5,
		.hp = 300,
		.strength = 10,
		.chance = 0,
		.rending = 0
	});

	// Make target move away so it will be attacked one time
	env.executeCommand(sw::io::March{
		.unitId = 3,
		.targetX = 5,
		.targetY = 6
	});

	env.clearOutput();

	// Run rounds and count all damage to unit 3
	int totalDamage = 0;
	int damagePerTick = 0;
	int ticksObserved = 0;

	for (int round = 0; round < 12; ++round)
	{
		env.world.round(env.events);

		env.resetOutput();
		while (auto eventOpt = env.parseEvent<sw::io::UnitAttacked>())
		{
			const auto& [tick, attack] = *eventOpt;
			if (attack.targetUnitId == 3)
			{
				totalDamage += attack.damage;
				ticksObserved++;

				// Track damage per tick (should be max of the two poisons)
				damagePerTick = std::max<uint32_t>(attack.damage, damagePerTick);
			}
		}

		env.clearOutput();
	}

	// Both poisons should have been applied and their total damage stacked
	// Total damage should be 30 + 40 = 70
	REQUIRE(totalDamage == 70);

	// Damage per tick should be the maximum of the two: max(6, 8) = 8
	REQUIRE(damagePerTick == 8);

	// Total ticks should be 70 / 8 = 9
	REQUIRE(ticksObserved == 9);
}
