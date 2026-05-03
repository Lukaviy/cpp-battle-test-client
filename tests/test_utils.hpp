#pragma once

#include "Game/Core/Simulation/World.hpp"
#include "Game/Core/Registrator.hpp"
#include "Game/Features/Registry.hpp"
#include "IO/Commands/CreateMap.hpp"
#include "IO/Commands/March.hpp"
#include "IO/System/CommandParser.hpp"
#include "IO/System/EventSystem.hpp"
#include "IO/System/Details/FormatCommandVisitor.hpp"
#include "IO/System/Details/ParseEventVisitor.hpp"

#include <sstream>
#include <string>
#include <memory>
#include <vector>
#include <stdexcept>
#include <optional>

namespace test_utils
{
    struct TestEnvironment
    {
        std::stringstream output;
        sw::EventSystem events{output};
        std::mt19937 random;
        sw::core::World world{random};
        sw::io::CommandParser parser;
        sw::core::EventHandlerRegistry eventRegistry;

        TestEnvironment(unsigned int seed = 0)
        {
            if (seed != 0)
            {
                random.seed(seed);
            }

            sw::core::Registrator registrator{parser, world, events, eventRegistry};
            sw::game::registerAll(registrator);

            // Register system commands
            parser.add<sw::io::CreateMap>(
                [this](const sw::io::CreateMap& command)
                {
                    world.createMap(command.width, command.height);
                });

            parser.add<sw::io::March>(
                [this](const sw::io::March& command)
                {
                    world.march({command.targetX, command.targetY}, sw::game::UnitId{command.unitId}, events);
                });

            world.setEventRegistry(std::move(eventRegistry).createEventHandlerSystem());
        }

        // Execute typed command
        template<typename TCommand>
        void executeCommand(const TCommand& command)
        {
            std::string commandStr = sw::io::formatCommand(command);
            commandStr += '\n'; // Parser expects lines
            std::istringstream iss(commandStr);
            parser.parse(iss);
        }

        void removeUnit(std::uint32_t unitId)
        {
			sw::core::CommandBuffer commandBuffer;
            commandBuffer.removeEntity(world.getEntityIdByUnitId(sw::game::UnitId{unitId}).value());
            commandBuffer.performCommands(sw::core::CommandContext{world, events, world.currentTick()});
        }

        // Parse next event of specific type from output
        // Returns optional with tick and event if found, nullopt otherwise
        // Throws std::runtime_error if event line is malformed
        template<typename TEvent>
        std::optional<std::pair<std::uint64_t, TEvent>> parseEvent()
        {
            std::string line;

            while (std::getline(output, line))
            {
                // Event format: [tick] EVENT_NAME field1=value1 field2=value2...
                // Find the event name after [tick]
                size_t bracketStart = line.find('[');
                size_t bracketEnd = line.find(']');
                if (bracketStart == std::string::npos || bracketEnd == std::string::npos)
                {
                    throw std::runtime_error("Invalid event format: missing tick brackets in line: " + line);
                }

                // Parse tick
                std::uint64_t tick = 0;
                try
                {
                    std::string tickStr = line.substr(bracketStart + 1, bracketEnd - bracketStart - 1);
                    tick = std::stoull(tickStr);
                }
                catch (const std::exception& e)
                {
                    throw std::runtime_error("Invalid event format: failed to parse tick in line: " + line + " (" + e.what() + ")");
                }

                size_t eventNameStart = bracketEnd + 2; // Skip "] "
                if (eventNameStart >= line.length())
                {
                    throw std::runtime_error("Invalid event format: missing event name after tick in line: " + line);
                }

                std::string restOfLine = line.substr(eventNameStart);

                // Check if line contains the event name at the start
                if (restOfLine.find(TEvent::Name) == 0)
                {
                    try
                    {
                        // Extract event data part (after event name and space)
                        size_t dataStart = std::string(TEvent::Name).length();
                        std::string eventData;

                        if (dataStart < restOfLine.length() && restOfLine[dataStart] == ' ')
                        {
                            dataStart++; // Skip space
                            eventData = restOfLine.substr(dataStart);
                        }

                        TEvent event = sw::io::parseEvent<TEvent>(eventData);
                        return std::make_pair(tick, std::move(event));
                    }
                    catch (const std::exception& e)
                    {
                        throw std::runtime_error("Failed to parse event '" + std::string(TEvent::Name) + "' from line: " + line + " (" + e.what() + ")");
                    }
                }
            }

            return std::nullopt;
        }

        // Check if specific event exists in remaining output
        // Throws std::runtime_error if event line is malformed
        template<typename TEvent>
        bool hasEvent()
        {
            // Save current position
            auto pos = output.tellg();

            std::string line;
            bool found = false;

            while (std::getline(output, line))
            {
                // Event format: [tick] EVENT_NAME ...
                size_t bracketEnd = line.find(']');
                if (bracketEnd == std::string::npos)
                {
                    // Restore position before throwing
                    output.clear();
                    output.seekg(pos);
                    throw std::runtime_error("Invalid event format: missing tick bracket in line: " + line);
                }

                size_t eventNameStart = bracketEnd + 2; // Skip "] "
                if (eventNameStart >= line.length())
                {
                    // Restore position before throwing
                    output.clear();
                    output.seekg(pos);
                    throw std::runtime_error("Invalid event format: missing event name after tick in line: " + line);
                }

                std::string restOfLine = line.substr(eventNameStart);
                if (restOfLine.find(TEvent::Name) == 0)
                {
                    found = true;
                    break;
                }
            }

            // Restore position
            output.clear();
            output.seekg(pos);

            return found;
        }

        // Clear output buffer and reset read position
        void clearOutput()
        {
            output.str("");
            output.clear();
        }

        // Reset read position to beginning
        void resetOutput()
        {
            output.clear();
            output.seekg(0);
        }
    };

    // Helper for manual command parsing (for compatibility)
    inline void parseCommand(sw::io::CommandParser& parser, const std::string& command)
    {
        std::istringstream iss(command);
        parser.parse(iss);
    }
}
