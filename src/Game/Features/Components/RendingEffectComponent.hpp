#pragma once

namespace sw::core
{
	class Registrator;
}

namespace sw::game::component
{
	struct RendingEffectComponent
	{
	};

	void registerRendingEffectComponent(core::Registrator& registrator);
}
