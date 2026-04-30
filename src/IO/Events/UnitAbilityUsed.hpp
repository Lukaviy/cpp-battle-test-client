#pragma once

#include <cstdint>
#include <string>

namespace sw::io
{
	struct UnitAbilityUsed
	{
		constexpr static const char* Name = "UNIT_ABILITY_USED";

		uint32_t abilityUnitId{};
		std::string abilityName{};

		template <typename Visitor>
		void visit(Visitor& visitor)
		{
			visitor.visit("abilityUnitId", abilityUnitId);
			visitor.visit("abilityName", abilityName);
		}
	};
}
