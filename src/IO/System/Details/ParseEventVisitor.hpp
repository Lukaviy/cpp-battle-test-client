#pragma once

#include <sstream>
#include <string>
#include <stdexcept>

namespace sw::io
{
    class ParseEventVisitor
    {
    public:
        explicit ParseEventVisitor(const std::string& eventString)
            : _stream(eventString)
        {
        }

        template <typename T>
        void visit(const char* name, T& value)
        {
            // Find "name="
            std::string fieldName = name;
            std::string line;

            // Read current state
            std::streampos currentPos = _stream.tellg();
            std::string remaining;
            std::getline(_stream, remaining);
            _stream.seekg(currentPos);

            // Find field in remaining string
            size_t pos = remaining.find(fieldName + "=");
            if (pos == std::string::npos)
            {
                throw std::runtime_error("Field not found: " + fieldName);
            }

            // Extract value
            pos += fieldName.length() + 1; // skip "name="
            size_t endPos = remaining.find(' ', pos);
            if (endPos == std::string::npos)
            {
                endPos = remaining.length();
            }

            std::string valueStr = remaining.substr(pos, endPos - pos);

            // Parse value based on type
            if constexpr (std::is_same_v<T, std::string>)
            {
                value = valueStr;
            }
            else if constexpr (std::is_integral_v<T>)
            {
                if constexpr (std::is_unsigned_v<T>)
                {
                    value = static_cast<T>(std::stoull(valueStr));
                }
                else
                {
                    value = static_cast<T>(std::stoll(valueStr));
                }
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                value = static_cast<T>(std::stod(valueStr));
            }
        }

    private:
        std::istringstream _stream;
    };

    template <typename TEvent>
    TEvent parseEvent(const std::string& eventString)
    {
        TEvent event;
        ParseEventVisitor visitor(eventString);
        event.visit(visitor);
        return event;
    }
}
