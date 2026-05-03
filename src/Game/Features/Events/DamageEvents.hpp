#pragma once
#include "Game/Core/Entity.hpp"
#include "Game/Features/Damage.hpp"

namespace sw::game::event
{
	struct CollectDealDamageEvent
	{
		Damage damage;
	};

	struct CollectReceiveDamageEvent
	{
		Damage damage;
	};

	struct ReceiveDamageEvent
	{
		Damage damage;
		core::EntityId sourceEntity;
	};
}
