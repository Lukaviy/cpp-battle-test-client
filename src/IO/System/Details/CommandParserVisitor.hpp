#pragma once

#include <iostream>

namespace sw
{
	class CommandParserVisitor
	{
	public:
		explicit CommandParserVisitor(std::istream& stream) :
				_stream(stream)
		{}

		template <class TField>
		void visit(const char*, TField& field)
		{
			_stream >> field;
		}

	private:
		std::istream& _stream;
	};
}
