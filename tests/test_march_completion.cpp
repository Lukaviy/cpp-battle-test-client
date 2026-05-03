#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include "test_utils.hpp"
#include "IO/Commands/CreateMap.hpp"
#include "IO/Commands/SpawnSwordsman.hpp"
#include "IO/Commands/March.hpp"
#include "IO/Events/MarchEnded.hpp"

TEST_CASE("March ended event is emitted when destination is reached", "[movement][march][completion]")
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

    // Give march command - move 2 cells (from 2,2 to 4,4)
    env.executeCommand(sw::io::March{
        .unitId = 1,
        .targetX = 4,
        .targetY = 4
    });

    env.clearOutput();

    // Run rounds until march ends (should take 2 rounds for diagonal movement)
    bool marchEnded = false;
    for (int i = 0; i < 5 && !marchEnded; ++i)
    {
        env.world.round(env.events);
        env.resetOutput();
        marchEnded = env.hasEvent<sw::io::MarchEnded>();
    }

    REQUIRE(marchEnded);

    // Parse the march ended event
    env.resetOutput();
    std::optional<std::pair<std::uint64_t, sw::io::MarchEnded>> eventOpt;

    // Find last MarchEnded event
    while (auto nextEvent = env.parseEvent<sw::io::MarchEnded>())
    {
        eventOpt = nextEvent;
    }

    REQUIRE(eventOpt.has_value());

    const auto& [tick, marchEvent] = *eventOpt;
    REQUIRE(marchEvent.unitId == 1);
    REQUIRE(marchEvent.x == 4);
    REQUIRE(marchEvent.y == 4);
}
