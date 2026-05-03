#pragma once

namespace sw::core
{
	class Registrator;
}

namespace sw::game::component
{
	struct HealthComponent
	{
		int health{};
	};

	void registerHealthComponent(core::Registrator& registrator);
}
