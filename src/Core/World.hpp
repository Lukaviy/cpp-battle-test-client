#pragma once

#include "Component.hpp"
#include "DamageAccumulator.hpp"
#include "Entity.hpp"
#include "Ids.hpp"
#include "Interfaces.hpp"
#include "Map.hpp"
#include "Position.hpp"
#include "Random.hpp"

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <vector>

namespace sw
{
    class EventSystem;
}

namespace sw::core
{
    // Owns entities, the map, the RNG and the IO event bus.
    // Drives the per-tick simulation loop and provides spatial helpers.
    class World
    {
    public:
        World(EventSystem& events, std::unique_ptr<IRandom> random) :
                _events(events),
                _random(std::move(random))
        {}

        [[nodiscard]]
        EventSystem& events() noexcept
        {
            return _events;
        }

        [[nodiscard]]
        IRandom& random() noexcept
        {
            return *_random;
        }

        [[nodiscard]]
        Map& map() noexcept
        {
            return _map;
        }

        [[nodiscard]]
        const Map& map() const noexcept
        {
            return _map;
        }

        [[nodiscard]]
        std::uint64_t tick() const noexcept
        {
            return _tick;
        }

        [[nodiscard]]
        DamageAccumulator& damageAccumulator() noexcept
        {
            return _damageAccumulator;
        }

        void createMap(std::uint32_t width, std::uint32_t height)
        {
            _map = Map(width, height);
        }

        Entity& createEntity(EntityId id)
        {
            if (findEntity(id) != nullptr)
            {
                throw std::runtime_error("Entity with this id already exists");
            }
            auto owned = std::make_unique<Entity>(id, *this);
            Entity* raw = owned.get();
            _entities.push_back(std::move(owned));
            return *raw;
        }

        void placeEntity(Entity& entity, Position pos)
        {
            _map.place(entity.getId(), pos, isBlocking(entity));
        }

        void moveEntity(Entity& entity, Position to)
        {
            _map.move(entity.getId(), to, isBlocking(entity));
        }

        Entity* findEntity(EntityId id) noexcept
        {
            for (auto& e : _entities)
            {
                if (e->getId() == id && !e->isPendingDestroy())
                {
                    return e.get();
                }
            }
            return nullptr;
        }

        const Entity* findEntity(EntityId id) const noexcept
        {
            return const_cast<World*>(this)->findEntity(id);
        }

        void queryAdjacent(EntityId selfId, Position from, std::vector<EntityId>& out) const
        {
            out.clear();
            _map.forEachEntity(
                    [&](EntityId id, Position pos)
                    {
                        if (id == selfId)
                        {
                            return;
                        }
                        if (Map::chebyshev(from, pos) == 1)
                        {
                            out.push_back(id);
                        }
                    });
        }

        void queryRing(
                EntityId selfId,
                Position from,
                std::uint32_t minR,
                std::uint32_t maxR,
                std::vector<EntityId>& out) const
        {
            out.clear();
            _map.forEachEntity(
                    [&](EntityId id, Position pos)
                    {
                        if (id == selfId)
                        {
                            return;
                        }
                        const auto d = Map::chebyshev(from, pos);
                        if (d >= minR && d <= maxR)
                        {
                            out.push_back(id);
                        }
                    });
        }

        void destroyEntity(EntityId id)
        {
            if (Entity* e = findEntity(id); e != nullptr)
            {
                e->markPendingDestroy();
                _map.remove(id);
            }
        }

        bool step()
        {
            ++_tick;
            bool anyProgress = false;

            for (std::size_t i = 0; i < _entities.size(); ++i)
            {
                Entity* entity = _entities[i].get();
                if (entity == nullptr || entity->isPendingDestroy())
                {
                    continue;
                }

                entity->emit<&ITurnTick::onTurnTick>(*this, *entity);
                _damageAccumulator.flush(_events, _tick);
                if (entity->isPendingDestroy())
                {
                    continue;
                }

                std::vector<ITurnAction*> actions;
                entity->forEach<ITurnAction>([&](ITurnAction& a) { actions.push_back(&a); });
                std::stable_sort(
                        actions.begin(),
                        actions.end(),
                        [](ITurnAction* a, ITurnAction* b) { return a->priority() < b->priority(); });

                for (ITurnAction* action : actions)
                {
                    if (entity->isPendingDestroy())
                    {
                        break;
                    }
                    if (action->tryExecute(*this, *entity))
                    {
                        anyProgress = true;
                        break;
                    }
                }

                _damageAccumulator.flush(_events, _tick);
            }

            compactEntities();
            return anyProgress;
        }

        [[nodiscard]]
        bool isQuiescent() noexcept
        {
            if (!_map.isValid())
            {
                return true;
            }
            for (auto& e : _entities)
            {
                if (e->isPendingDestroy())
                {
                    continue;
                }
                if (e->get<ITurnAction>() != nullptr || e->get<ITurnTick>() != nullptr)
                {
                    return false;
                }
            }
            return true;
        }

    private:
        [[nodiscard]]
        bool isBlocking(Entity& entity) const
        {
            if (auto* spatial = entity.get<ISpatial>())
            {
                return spatial->blocksTile();
            }
            return true;
        }

        void compactEntities()
        {
            std::erase_if(
                    _entities,
                    [](const std::unique_ptr<Entity>& e) { return e == nullptr || e->isPendingDestroy(); });
            for (auto& e : _entities)
            {
                e->compact();
            }
        }

        EventSystem& _events;
        std::unique_ptr<IRandom> _random;
        Map _map;
        std::vector<std::unique_ptr<Entity>> _entities;
        DamageAccumulator _damageAccumulator;
        std::uint64_t _tick{0};
    };
}
