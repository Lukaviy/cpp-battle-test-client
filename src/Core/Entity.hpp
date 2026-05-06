#pragma once

#include "Component.hpp"
#include "Ids.hpp"
#include "Tools/InterfaceList.hpp"
#include "Tools/MethodTraits.hpp"

#include <cassert>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace sw::core
{
    class World;

    class Entity
    {
    public:
        Entity(EntityId id, World& world) :
                _id(id),
                _world(&world)
        {}

        ~Entity() = default;

        Entity(const Entity&) = delete;
        Entity& operator=(const Entity&) = delete;
        Entity(Entity&&) = delete;
        Entity& operator=(Entity&&) = delete;

        [[nodiscard]]
        EntityId getId() const noexcept
        {
            return _id;
        }

        [[nodiscard]]
        World& getWorld() const noexcept
        {
            return *_world;
        }

        [[nodiscard]]
        bool isPendingDestroy() const noexcept
        {
            return _pendingDestroy;
        }

        void markPendingDestroy() noexcept
        {
            _pendingDestroy = true;
        }

        // --- add / remove ---------------------------------------------------

        template <class C, class... Args>
            requires std::is_base_of_v<BaseComponent, C>
        C& add(Args&&... args)
        {
            auto owned = std::make_unique<C>(std::forward<Args>(args)...);
            C* raw = owned.get();
            raw->_owner = this;
            raw->_world = _world;

            Slot slot;
            slot.typeId = ComponentTypeIdRegistry::getId<C>();
            slot.base = raw;
            slot.owned = std::move(owned);
            _components.push_back(std::move(slot));

            registerInterfaces<C>(raw, typename C::ImplementedInterfaces{});
            return *raw;
        }

        // Marks component(s) of type C as not alive and schedules deletion.
        // Actual memory release happens after the current top-level emit / step boundary.
        template <class C>
            requires std::is_base_of_v<BaseComponent, C>
        void remove()
        {
            const auto typeId = ComponentTypeIdRegistry::getId<C>();
            for (auto& slot : _components)
            {
                if (slot.typeId == typeId && slot.base != nullptr && slot.base->_alive)
                {
                    slot.base->_alive = false;
                }
            }
        }

        // --- query ----------------------------------------------------------

        template <class T>
        [[nodiscard]]
        T* get() noexcept
        {
            const auto typeId = ComponentTypeIdRegistry::getId<T>();
            if constexpr (std::is_base_of_v<BaseComponent, T>)
            {
                for (auto& slot : _components)
                {
                    if (slot.typeId == typeId && slot.base != nullptr && slot.base->_alive)
                    {
                        return static_cast<T*>(slot.base);
                    }
                }
                return nullptr;
            }
            else
            {
                if (typeId.get_raw() >= _byInterface.size())
                {
                    return nullptr;
                }
                for (const auto& entry : _byInterface[typeId.get_raw()])
                {
                    if (entry.base != nullptr && entry.base->_alive)
                    {
                        return static_cast<T*>(entry.iface);
                    }
                }
                return nullptr;
            }
        }

        template <class T>
        [[nodiscard]]
        const T* get() const noexcept
        {
            return const_cast<Entity*>(this)->get<T>();
        }

        // Visit every alive implementer of an interface or every component of a concrete type.
        // Snapshot semantics: handlers added during traversal are not visited; removed are skipped.
        template <class T, class F>
        void forEach(F&& fn)
        {
            visitSnapshot<T>(
                    [&](T* p, BaseComponent* /*base*/)
                    {
                        fn(*p);
                        return false;
                    });
        }

        // --- event dispatch -------------------------------------------------

        // Broadcast a void method to every alive implementer of the owning interface.
        // Method must return void (multiple handlers with non-void return is ill-defined).
        template <auto Method, class... Args>
        void emit(Args&&... args)
        {
            using MTraits = MethodTraits<decltype(Method)>;
            using Iface = typename MTraits::class_type;
            static_assert(std::is_void_v<typename MTraits::return_type>, "emit<> requires a void-returning method");

            visitSnapshot<Iface>(
                    [&](Iface* p, BaseComponent* /*base*/)
                    {
                        (p->*Method)(args...);
                        return false;
                    });
        }

        // Call the method on the first alive implementer (in registration order).
        // If no implementer exists, returns a value-initialized return value (or nothing for void).
        template <auto Method, class... Args>
        auto callFirst(Args&&... args)
        {
            using MTraits = MethodTraits<decltype(Method)>;
            using Iface = typename MTraits::class_type;
            using Ret = typename MTraits::return_type;

            if constexpr (std::is_void_v<Ret>)
            {
                visitSnapshot<Iface>(
                        [&](Iface* p, BaseComponent* /*base*/)
                        {
                            (p->*Method)(std::forward<Args>(args)...);
                            return true;
                        });
            }
            else
            {
                Ret result{};
                visitSnapshot<Iface>(
                        [&](Iface* p, BaseComponent* /*base*/)
                        {
                            result = (p->*Method)(std::forward<Args>(args)...);
                            return true;
                        });
                return result;
            }
        }

        // Call the method on each alive implementer until one returns true.
        // Returns true if any handler accepted the call.
        template <auto Method, class... Args>
        bool callAny(Args&&... args)
        {
            using MTraits = MethodTraits<decltype(Method)>;
            using Iface = typename MTraits::class_type;
            static_assert(
                    std::is_same_v<typename MTraits::return_type, bool>, "callAny<> requires a bool-returning method");

            bool accepted = false;
            visitSnapshot<Iface>(
                    [&](Iface* p, BaseComponent* /*base*/)
                    {
                        if ((p->*Method)(args...))
                        {
                            accepted = true;
                            return true;
                        }
                        return false;
                    });
            return accepted;
        }

        // --- maintenance ----------------------------------------------------

        // Removes physically-destroyed slots. Called by World between top-level emits / steps.
        void compact()
        {
            std::erase_if(_components, [](const Slot& s) { return s.base == nullptr || !s.base->_alive; });
            for (auto& bucket : _byInterface)
            {
                std::erase_if(bucket, [](const InterfaceEntry& e) { return e.base == nullptr || !e.base->_alive; });
            }
        }

    private:
        struct Slot
        {
            ComponentTypeId typeId{0};
            BaseComponent* base{nullptr};
            std::unique_ptr<BaseComponent> owned;
        };

        struct InterfaceEntry
        {
            void* iface{nullptr};
            BaseComponent* base{nullptr};
        };

        template <class C, class... I>
        void registerInterfaces(C* raw, InterfaceList<I...>)
        {
            (registerInterface<I>(raw), ...);
        }

        template <class I, class C>
        void registerInterface(C* raw)
        {
            static_assert(std::is_base_of_v<I, C>, "Component does not actually implement listed interface");
            const auto typeId = ComponentTypeIdRegistry::getId<I>();
            if (typeId.get_raw() >= _byInterface.size())
            {
                _byInterface.resize(typeId.get_raw() + 1);
            }
            InterfaceEntry entry;
            entry.iface = static_cast<I*>(raw);
            entry.base = raw;
            _byInterface[typeId.get_raw()].push_back(entry);
        }

        // Common snapshot loop. Visitor returns true to stop iteration.
        template <class T, class Visitor>
        void visitSnapshot(Visitor&& visit)
        {
            if constexpr (std::is_base_of_v<BaseComponent, T>)
            {
                const auto typeId = ComponentTypeIdRegistry::getId<T>();
                std::vector<InterfaceEntry> snapshot;
                for (const auto& slot : _components)
                {
                    if (slot.typeId == typeId && slot.base != nullptr)
                    {
                        snapshot.push_back({slot.base, slot.base});
                    }
                }
                for (const auto& entry : snapshot)
                {
                    if (entry.base == nullptr || !entry.base->_alive)
                    {
                        continue;
                    }
                    if (visit(static_cast<T*>(entry.base), entry.base))
                    {
                        return;
                    }
                }
            }
            else
            {
                const auto typeId = ComponentTypeIdRegistry::getId<T>();
                if (typeId.get_raw() >= _byInterface.size())
                {
                    return;
                }
                const auto snapshot = _byInterface[typeId.get_raw()];
                for (const auto& entry : snapshot)
                {
                    if (entry.base == nullptr || !entry.base->_alive)
                    {
                        continue;
                    }
                    if (visit(static_cast<T*>(entry.iface), entry.base))
                    {
                        return;
                    }
                }
            }
        }

        EntityId _id;
        World* _world{nullptr};
        std::vector<Slot> _components;
        std::vector<std::vector<InterfaceEntry>> _byInterface;
        bool _pendingDestroy{false};
    };
}
