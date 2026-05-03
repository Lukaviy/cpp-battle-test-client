#include "Attack.hpp"

#include "Game/Features/Events/DamageEvents.hpp"
#include "Game/Core/Event/EventHandlerSystem.hpp"

namespace sw::game::behaviour
{
	Damage collectDealDamage(const core::EventContext& context, core::EntityId entityId, const Damage& damage)
	{
		auto event = event::CollectDealDamageEvent{.damage = damage};
		context.eventRegistry.emit(event, context, entityId);
		return event.damage;
	}

	Damage collectReceiveDamage(const core::EventContext& context, core::EntityId entityId, const Damage& damage)
	{
		auto event = event::CollectReceiveDamageEvent{.damage = damage};
		context.eventRegistry.emit(event, context, entityId);
		return event.damage;
	}

	void receiveDamage(const core::EventContext& context, core::EntityId entityId, core::EntityId sourceEntityId, const Damage& damage)
	{
		const auto full_damage = collectReceiveDamage(context, entityId, damage);
		context.eventRegistry.emit(
				event::ReceiveDamageEvent{.damage = full_damage, .sourceEntity = sourceEntityId}, context, entityId);
	}

	void attack(const core::EventContext& context, core::EntityId entityId, core::EntityId targetEntityId, const Damage& damage)
	{
		const auto full_damage = collectDealDamage(context, entityId, damage);

		receiveDamage(context, targetEntityId, entityId, full_damage);
	}
}

namespace sw::game
{
	std::int32_t totalDamage(const Damage& damage)
	{
		std::int32_t total = 0;

		for (size_t i = 0; i < damage.values.size(); ++i)
		{
			total += damage.values[i] * damage.multipliers[i] / 1000;
		}

		return total;
	}

	Damage::Damage()
	{
		std::ranges::fill(multipliers, 1000);
	}

	Damage& Damage::operator+=(const Damage& other)
	{
		for (size_t i = 0; i < values.size(); ++i)
		{
			values[i] += other.values[i];
			multipliers[i] += other.multipliers[i];
		}
		return *this;
	}

	Damage operator+(Damage lhs, const Damage& rhs)
	{
		lhs += rhs;
		return lhs;
	}
}
