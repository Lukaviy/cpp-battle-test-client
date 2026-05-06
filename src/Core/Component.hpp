#pragma once

#include "Ids.hpp"
#include "Tools/InterfaceList.hpp"

namespace sw::core
{
    class Entity;
    class World;

    // Marker base for all components. Owned by Entity through unique_ptr.
    class BaseComponent
    {
    public:
        virtual ~BaseComponent() = default;

        [[nodiscard]]
        Entity* getOwner() const noexcept
        {
            return _owner;
        }

        [[nodiscard]]
        World* getWorld() const noexcept
        {
            return _world;
        }

        [[nodiscard]]
        bool isAlive() const noexcept
        {
            return _alive;
        }

    private:
        friend class Entity;

        Entity* _owner{nullptr};
        World* _world{nullptr};
        bool _alive{true};
    };

    // CRTP base for components.
    // Usage: class PoisonEffect : public Component<IEffect, ITurnReceiver> { ... };
    // The list is also exposed as ImplementedInterfaces for the Entity registry.
    template <class... Interfaces>
    class Component : public BaseComponent, public Interfaces...
    {
    public:
        using ImplementedInterfaces = InterfaceList<Interfaces...>;
    };

    // Component without any interface (data-only or only used by concrete type).
    using DataComponent = Component<>;
}
