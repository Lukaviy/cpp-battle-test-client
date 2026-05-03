#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include "test_utils.hpp"
#include "IO/Commands/CreateMap.hpp"
#include "IO/Commands/SpawnSwordsman.hpp"
#include "IO/Events/UnitSpawned.hpp"

TEST_CASE("Unit spawned event is emitted", "[unit][spawn]")
{
    test_utils::TestEnvironment env;

    // Create map using typed command
    sw::io::CreateMap createMapCmd{.width = 10, .height = 10};
    env.executeCommand(createMapCmd);

    // Clear output to only capture spawn event
    env.clearOutput();

    // Spawn a swordsman using typed command
    sw::io::SpawnSwordsman spawnCmd{
        .unitId = 1,
        .x = 5,
        .y = 5,
        .hp = 100,
        .strength = 10,
        .chance = 0,
        .rending = 0
    };
    env.executeCommand(spawnCmd);

    // Parse the spawn event
    env.resetOutput();
    auto eventOpt = env.parseEvent<sw::io::UnitSpawned>();

    REQUIRE(eventOpt.has_value());

    const auto& [tick, spawnEvent] = *eventOpt;
    REQUIRE(spawnEvent.unitId == 1);
    REQUIRE(spawnEvent.x == 5);
    REQUIRE(spawnEvent.y == 5);
    REQUIRE_FALSE(spawnEvent.unitType.empty());
}
