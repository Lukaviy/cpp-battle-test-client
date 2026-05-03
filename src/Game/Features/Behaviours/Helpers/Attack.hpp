#pragma once
#include "Game/Core/Entity.hpp"
#include "Game/Features/Damage.hpp"

namespace sw::core
{
	struct EventContext;
}

namespace sw::game::behaviour
{
	Damage collectDealDamage(const core::EventContext& context, core::EntityId entityId, const Damage& damage);
	Damage collectReceiveDamage(const core::EventContext& context, core::EntityId entityId, const Damage& damage);

	void receiveDamage(
			const core::EventContext& context,
			core::EntityId entityId,
			core::EntityId sourceEntityId,
			const Damage& damage);

	void attack(
			const core::EventContext& context,
			core::EntityId entityId,
			core::EntityId targetEntityId,
			const Damage& damage);
}
