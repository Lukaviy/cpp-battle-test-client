#pragma once

#include <sstream>
#include <string>

namespace sw::io
{
    class FormatCommandVisitor
    {
    public:
        FormatCommandVisitor() = default;

        template <typename T>
        void visit(const char* name, const T& value)
        {
            // CommandParserVisitor just reads values without field names
            _stream << value << ' ';
        }

        std::string toString() const
        {
            return _stream.str();
        }

    private:
        std::ostringstream _stream;
    };

    template <typename TCommand>
    std::string formatCommand(const TCommand& command)
    {
        FormatCommandVisitor visitor;
        std::ostringstream result;
        result << TCommand::Name << ' ';

        // Cast away const to call visit - visitor only reads fields
        TCommand& mutableCommand = const_cast<TCommand&>(command);
        mutableCommand.visit(visitor);

        result << visitor.toString();
        return result.str();
    }
}
