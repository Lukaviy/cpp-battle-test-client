#include "Random.hpp"

namespace sw::core
{
	bool checkChance(std::mt19937& random, int chance)
	{
		std::uniform_int_distribution<> distribution(0, 1000);
		return distribution(random) < chance;
	}
}
