#pragma once

#include "Details/PrintFieldVisitor.hpp"

#include <iostream>

namespace sw
{
	class EventSystem
	{
	public:
		explicit EventSystem(std::ostream& stream)
			: _stream(stream)
		{
		}

		template <class TEvent>
		void event(const uint64_t tick, TEvent&& event)
		{
			_stream << "[" << tick << "] " << TEvent::Name << " ";
			PrintFieldVisitor visitor(_stream);
			event.visit(visitor);
			_stream << std::endl;
		}

	private:
		std::ostream& _stream;
	};
}
