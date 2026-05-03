#pragma once
#include <cstdint>

namespace sw::core
{
	class Registrator;
}

namespace sw::game::behaviour
{
	struct HunterBehaviour
	{
		std::int32_t agility{};
		std::int32_t strength{};
		std::int32_t range{};
		std::int32_t chance{};
		std::int32_t poison{};
	};

	void registerHunter(core::Registrator& registrator);
}
