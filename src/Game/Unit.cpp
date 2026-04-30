#include "Unit.hpp"

#include <random>

namespace sw::game
{
	std::random_device randomDevice;
	std::mt19937 random(randomDevice());
	std::uniform_int_distribution<std::mt19937::result_type> randomDistribution(1, 1000);

	Unit::Unit(
			const uint32_t id,
			const Position currentPosition,
			const uint32_t hp,
			const uint32_t strength,
			const uint32_t chance,
			const uint32_t rending) :
			_id(id),
			_currentPosition(currentPosition),
			_hp(hp),
			_strength(strength),
			_chance(chance),
			_rending(rending)
	{}

	bool Unit::isActive() const
	{
		if (_hp == 0)
		{
			return false;
		}
		if (_targetPosition.has_value() && _currentPosition != _targetPosition.value())
		{
			return true;
		}
		return !_idle;
	}

	bool Unit::isAlive() const
	{
		return _hp > 0;
	}

	bool Unit::tryGetTargetPosition(Position& targetPosition) const
	{
		if (_targetPosition.has_value())
		{
			targetPosition = _targetPosition.value();
			return true;
		}
		return false;
	}

	void Unit::attack(Unit& target) const
	{
		target.applyDamage(_strength);
	}

	void Unit::applyDamage(const uint32_t damage)
	{
		if (_hp > damage)
		{
			_hp -= damage;
		}
		else
		{
			_hp = 0;
		}
	}

	bool Unit::tryApplyAbility(Unit& target) const
	{
		if (const uint32_t value = randomDistribution(random); value < _chance)
		{
			target.applyDamage(_rending);
			return true;
		}
		return false;
	}
}
