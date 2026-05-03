#include "Registry.hpp"

#include "IOEventsRegistry.hpp"
#include "Game/Features/Components/PoisonEffectComponent.hpp"
#include "Game/Features/Components/RendingEffectComponent.hpp"
#include "Game/Features/Behaviours/SwordsmanBehaviour.hpp"
#include "Game/Features/Behaviours/HunterBehaviour.hpp"
#include "Game/Features/Components/HealthComponent.hpp"

namespace sw::game
{
	void registerAll(core::Registrator& registrator)
	{
		// Units
		behaviour::registerSwordsman(registrator);
		behaviour::registerHunter(registrator);

		// Components
		component::registerPoisonEffectComponent(registrator);
		component::registerRendingEffectComponent(registrator);
		component::registerHealthComponent(registrator);

		// IO Events
		registerIOEventHandlers(registrator);
	}
}
