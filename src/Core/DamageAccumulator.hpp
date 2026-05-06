#pragma once

#include "Damage.hpp"
#include "Ids.hpp"
#include "IO/Events/UnitAttacked.hpp"
#include "IO/System/EventSystem.hpp"

#include <cstdint>
#include <unordered_map>
#include <utility>

namespace sw::core
{
    // Aggregates damage per (attacker, target) within a single emit-flush window
    // so the engine emits one UNIT_ATTACKED event with the total damage even
    // when multiple sources hit (e.g. poison ticks combined with rending doubling).
    class DamageAccumulator
    {
    public:
        void add(const Damage& damage, std::uint32_t targetHpAfter)
        {
            const Key key{damage.attackerId, damage.targetId};
            auto [it, inserted] = _entries.emplace(key, Entry{damage.amount, targetHpAfter});
            if (!inserted)
            {
                it->second.totalDamage += damage.amount;
                it->second.targetHpAfter = targetHpAfter;
            }
            _order.push_back(key);
        }

        void flush(EventSystem& events, std::uint64_t tick)
        {
            std::unordered_map<std::uint64_t, bool> emitted;
            for (const auto& key : _order)
            {
                const auto packed = pack(key);
                if (emitted[packed])
                {
                    continue;
                }
                emitted[packed] = true;
                const auto& entry = _entries.at(key);
                events.event(
                        tick,
                        io::UnitAttacked{
                                key.attackerId.get_raw(),
                                key.targetId.get_raw(),
                                entry.totalDamage,
                                entry.targetHpAfter});
            }
            _entries.clear();
            _order.clear();
        }

    private:
        struct Key
        {
            EntityId attackerId{0};
            EntityId targetId{0};

            bool operator==(const Key& other) const noexcept
            {
                return attackerId == other.attackerId && targetId == other.targetId;
            }
        };

        struct KeyHash
        {
            std::size_t operator()(const Key& k) const noexcept
            {
                return std::hash<EntityId>{}(k.attackerId) ^ (std::hash<EntityId>{}(k.targetId) << 1);
            }
        };

        struct Entry
        {
            std::uint32_t totalDamage{0};
            std::uint32_t targetHpAfter{0};
        };

        [[nodiscard]]
        std::uint64_t pack(const Key& k) const noexcept
        {
            return (static_cast<std::uint64_t>(k.attackerId.get_raw()) << 32) | k.targetId.get_raw();
        }

        std::unordered_map<Key, Entry, KeyHash> _entries;
        std::vector<Key> _order;
    };
}
