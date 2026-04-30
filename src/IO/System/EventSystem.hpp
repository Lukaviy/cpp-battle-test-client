#pragma once

#include "Details/PrintFieldVisitor.hpp"

#include <iostream>

namespace sw
{
	class EventSystem
	{
	public:
		template <class TEvent>
		void event(const uint64_t tick, TEvent&& event)
		{
			std::cout << "[" << tick << "] " << TEvent::Name << " ";
			PrintFieldVisitor visitor(std::cout);
			event.visit(visitor);
			std::cout << std::endl;
		}
	};
}
