#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include "test_utils.hpp"
#include "IO/Commands/CreateMap.hpp"
#include "IO/Commands/SpawnSwordsman.hpp"
#include "IO/Events/UnitAttacked.hpp"

TEST_CASE("Rending effect deals correct damage", "[effects][rending][combat]")
{
    // Use fixed seed for reproducibility
    test_utils::TestEnvironment env(12345);

    // Create map
    env.executeCommand(sw::io::CreateMap{.width = 10, .height = 10});

    // Spawn two swordsmen next to each other
    // Attacker with 100% chance of rending (chance=1000)
    env.executeCommand(sw::io::SpawnSwordsman{
        .unitId = 1,
        .x = 5,
        .y = 5,
        .hp = 100,
        .strength = 10,
        .chance = 1000,
        .rending = 20
    });
    // Target
    env.executeCommand(sw::io::SpawnSwordsman{
        .unitId = 2,
        .x = 6,
        .y = 5,
        .hp = 100,
        .strength = 10,
        .chance = 0,
        .rending = 0
    });

    env.clearOutput();

    // Run rounds until attack happens
    bool attackHappened = false;
    for (int i = 0; i < 5 && !attackHappened; ++i)
    {
        env.world.round(env.events);
        env.resetOutput();
        attackHappened = env.hasEvent<sw::io::UnitAttacked>();
    }

    REQUIRE(attackHappened);

    // Parse attack events and find attack from unit 1 to unit 2
    env.resetOutput();
    bool foundCorrectAttack = false;

    while (auto eventOpt = env.parseEvent<sw::io::UnitAttacked>())
    {
        const auto& [tick, attack] = *eventOpt;
        if (attack.attackerUnitId == 1 && attack.targetUnitId == 2)
        {
            foundCorrectAttack = true;
            // With 100% chance, rending damage (20) should be dealt
            REQUIRE(attack.damage == 20);
            break;
        }
    }

    REQUIRE(foundCorrectAttack);
}

