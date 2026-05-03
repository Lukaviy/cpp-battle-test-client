#include "ComponentsContainer.hpp"
#include "ComponentTypeRegistry.hpp"

#include <ranges>

namespace sw::core
{
	void ComponentsContainer::removeComponents(EntityId entity_id)
	{
		for (const auto& array : _componentArrays | std::views::values)
		{
			array->removeComponent(entity_id);
		}
	}

	void* ComponentsContainer::getComponent(EntityId entity_id, ComponentId component_id) const
	{
		const auto it = _componentArrays.find(component_id);

		if (it == _componentArrays.end())
		{
			return nullptr;
		}

		return it->second->getComponent(entity_id);
	}

	void ComponentsContainer::removeComponent(EntityId entity_id, ComponentId component_id)
	{
		const auto it = _componentArrays.find(component_id);

		if (it == _componentArrays.end())
		{
			return;
		}

		it->second->removeComponent(entity_id);
	}
}
