#pragma once

#include <cstdint>

namespace sw::io
{
	struct SpawnSwordsman
	{
		constexpr static const char* Name = "SPAWN_SWORDSMAN";

		uint32_t unitId{};
		uint32_t x{};
		uint32_t y{};
		uint32_t hp{};
		uint32_t strength{};
		uint32_t chance{};
		uint32_t rending{};

		template <typename Visitor>
		void visit(Visitor& visitor)
		{
			visitor.visit("unitId", unitId);
			visitor.visit("x", x);
			visitor.visit("y", y);
			visitor.visit("hp", hp);
			visitor.visit("strength", strength);
			visitor.visit("chance", chance);
			visitor.visit("rending", rending);
		}
	};
}
