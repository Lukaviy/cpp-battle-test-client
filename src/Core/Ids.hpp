#pragma once

#include "Tools/TypeRegistry.hpp"
#include "Tools/TypedId.hpp"

#include <cstdint>

namespace sw::core
{
    struct EntityIdTag;
    using EntityId = TypedId<EntityIdTag, std::uint32_t>;

    struct ComponentTypeIdTag;
    using ComponentTypeId = TypedId<ComponentTypeIdTag, std::uint32_t>;
    using ComponentTypeIdRegistry = TypeRegistry<ComponentTypeId>;
}
