#pragma once
#include <array>
#include <cstdint>

namespace sw::game
{
	enum class DamageKind : size_t
	{
		Physical,
		Poison,
		Fire,
		COUNT
	};

	template <DamageKind T>
	struct DamageUnit
	{
		std::int32_t value{};

		DamageUnit(std::int32_t value) :
				value(value)
		{}
	};

	struct Damage
	{
		std::array<std::int32_t, static_cast<size_t>(DamageKind::COUNT)> values{};
		std::array<std::int32_t, static_cast<size_t>(DamageKind::COUNT)> multipliers{};

		template <DamageKind T>
		Damage(DamageUnit<T> unit) :
				Damage()
		{
			values[static_cast<size_t>(T)] = unit.value;
		}

		Damage();

		template <DamageKind T>
		Damage& addMultiplier(DamageUnit<T> unit)
		{
			multipliers[static_cast<size_t>(T)] += unit.value;
			return *this;
		}

		Damage& operator+=(const Damage& other);
		friend Damage operator+(Damage lhs, const Damage& rhs);
	};

	std::int32_t totalDamage(const Damage& damage);
}
