#pragma once

#include "Position.hpp"

#include <optional>

namespace sw::game
{
	class Unit
	{
	public:
		Unit(uint32_t id, Position currentPosition, uint32_t hp, uint32_t strength, uint32_t chance, uint32_t rending);
		~Unit() = default;

		[[nodiscard]]
		bool isActive() const;
		[[nodiscard]]
		bool isAlive() const;

		[[nodiscard]]
		uint32_t getId() const
		{
			return _id;
		}

		[[nodiscard]]
		uint32_t getHp() const
		{
			return _hp;
		}

		[[nodiscard]]
		uint32_t getStrength() const
		{
			return _strength;
		}

		[[nodiscard]]
		uint32_t getRending() const
		{
			return _rending;
		}

		void setIdle(const bool value)
		{
			_idle = value;
		}

		[[nodiscard]]
		Position getCurrentPosition() const
		{
			return _currentPosition;
		}

		void setCurrentPosition(const Position currentPosition)
		{
			_currentPosition = currentPosition;
		}

		bool tryGetTargetPosition(Position& targetPosition) const;

		void setTargetPosition(const Position targetPosition)
		{
			_targetPosition = targetPosition;
		}

		void resetTargetPosition()
		{
			_targetPosition.reset();
		}

		void attack(Unit& target) const;
		void applyDamage(uint32_t damage);

		bool tryApplyAbility(Unit& target) const;

	private:
		uint32_t _id{0};
		Position _currentPosition;
		std::optional<Position> _targetPosition;
		uint32_t _hp{0};
		uint32_t _strength{0};
		uint32_t _chance{0};
		uint32_t _rending{0};
		bool _idle{true};
	};
}
