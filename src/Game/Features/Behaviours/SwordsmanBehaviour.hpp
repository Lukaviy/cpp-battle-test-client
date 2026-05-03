#pragma once
#include <cstdint>

namespace sw::core
{
	class Registrator;
}

namespace sw::game::behaviour
{
	struct SwordsmanBehaviour
	{
		std::int32_t strength{};
		std::int32_t rending{};
		std::int32_t rendingAbilityChance{};
	};

	void registerSwordsman(core::Registrator& registrator);
}
