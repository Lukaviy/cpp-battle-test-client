#pragma once

#include <cstdint>

namespace sw::io
{
	struct UnitDied
	{
		constexpr static auto Name = "UNIT_DIED";

		uint32_t unitId{};

		template <typename Visitor>
		void visit(Visitor& visitor)
		{
			visitor.visit("unitId", unitId);
		}
	};
}
