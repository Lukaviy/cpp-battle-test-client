#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include "test_utils.hpp"
#include "IO/Commands/CreateMap.hpp"
#include "IO/Commands/SpawnSwordsman.hpp"
#include "IO/Commands/March.hpp"
#include "IO/Events/MarchStarted.hpp"

TEST_CASE("March started event is emitted", "[movement][march]")
{
    test_utils::TestEnvironment env;

    // Create map
    env.executeCommand(sw::io::CreateMap{.width = 10, .height = 10});

    // Spawn a swordsman
    env.executeCommand(sw::io::SpawnSwordsman{
        .unitId = 1,
        .x = 2,
        .y = 2,
        .hp = 100,
        .strength = 10,
        .chance = 0,
        .rending = 0
    });

    env.clearOutput();

    // Give march command
    env.executeCommand(sw::io::March{
        .unitId = 1,
        .targetX = 5,
        .targetY = 5
    });

    // Run one round to start movement
    env.world.round(env.events);

    // Parse the march started event
    env.resetOutput();
    auto eventOpt = env.parseEvent<sw::io::MarchStarted>();

    REQUIRE(eventOpt.has_value());

    const auto& [tick, marchEvent] = *eventOpt;
    REQUIRE(marchEvent.unitId == 1);
    REQUIRE(marchEvent.targetX == 5);
    REQUIRE(marchEvent.targetY == 5);
}
