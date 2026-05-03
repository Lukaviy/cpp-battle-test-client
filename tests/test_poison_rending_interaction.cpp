#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include "test_utils.hpp"
#include "IO/Commands/CreateMap.hpp"
#include "IO/Commands/SpawnHunter.hpp"
#include "IO/Commands/SpawnSwordsman.hpp"
#include "IO/Events/UnitAttacked.hpp"

TEST_CASE("Poison and Rending effects interact correctly", "[effects][poison][rending][interaction]")
{
	// Use fixed seed for reproducibility
	test_utils::TestEnvironment env(12345);

	// Create map
	env.executeCommand(sw::io::CreateMap{.width = 10, .height = 10});

	 // Spawn hunter with 100% poison chance, 10 poison damage per turn
	env.executeCommand(
			sw::io::SpawnHunter{
					.unitId = 1,
					.x = 7,
					.y = 0,
					.hp = 100,
					.agility = 10,
					.strength = 5,
					.range = 5,
					.chance = 1000,
					.poison = 50});

	// Spawn swordsman with 100% rending chance next to target
	env.executeCommand(
			sw::io::SpawnSwordsman{
					.unitId = 2, .x = 1, .y = 0, .hp = 100, .strength = 10, .chance = 1000, .rending = 15});
	// Spawn target
	env.executeCommand(
			sw::io::SpawnSwordsman{
					.unitId = 3, .x = 2, .y = 0, .hp = 200, .strength = 10, .chance = 0, .rending = 0});

	env.executeCommand(
			sw::io::March{
					.unitId = 2, .targetX = 0, .targetY = 0});

	env.clearOutput();

	// Round 1: 
	// 1. Hunters turn: Poison damage from hunter's attack: 10
	// 2. Swordsman's turn: Applies rending to unit 2, which should double the poison damage for this turn: 10 (base poison) + 10 (rending bonus) = 20
	// 3. Swordsman's turn: Normal poison damage: 10
	// Poison damage applies every unit turn. So total poison damage for unit 2 in this round should be 10 (hunter's poison) + 20 (rending interaction) + 10 (normal poison) = 40
	env.world.round(env.events);

	env.resetOutput();
	int poisonDamage = 0;

	while (auto eventOpt = env.parseEvent<sw::io::UnitAttacked>())
	{
		const auto& [tick, attack] = *eventOpt;
		if (attack.targetUnitId == 3)
		{
			if (attack.attackerUnitId == 1) // Poison damage from hunter
			{
				poisonDamage += attack.damage;
			}
		}
	}

	REQUIRE(poisonDamage == 40);

	// Remove unit's so they won't attack in next rounds and we can observe poison damage without new attacks or rending interaction
	env.removeUnit(1);
	env.removeUnit(2);

	env.clearOutput();

	// Round 2+: Now poison should deal normal damage (10 per turn) without rending
	for (int round = 0; round < 10; ++round)
	{
		env.world.round(env.events);
	}

	env.resetOutput();
	int normalPoisonDamage = 0;

	while (auto eventOpt = env.parseEvent<sw::io::UnitAttacked>())
	{
		const auto& [tick, attack] = *eventOpt;
		if (attack.targetUnitId == 3 && attack.attackerUnitId == 1)
		{
			normalPoisonDamage += attack.damage;
		}
	}

	// Remaining poison ticks (2 turns * 10 damage) = 20
	REQUIRE(normalPoisonDamage == 20);
}
