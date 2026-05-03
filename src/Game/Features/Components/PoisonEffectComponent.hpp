#pragma once
#include "Game/Core/Entity.hpp"

namespace sw::core
{
	class Registrator;
}

namespace sw::game::component
{
	struct PoisonEffectComponent
	{
		core::EntityId sourceEntity;

		std::int32_t remainingDamage;
		std::int32_t damagePerTick;

		static PoisonEffectComponent fromTotalDamage(
				core::EntityId sourceEntity,
				std::int32_t totalDamage,
				std::int32_t tickCount);
	};

	void registerPoisonEffectComponent(core::Registrator& registrator);
}
