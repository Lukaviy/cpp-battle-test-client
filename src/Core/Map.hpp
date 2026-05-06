#pragma once

#include "Ids.hpp"
#include "Position.hpp"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace sw::core
{
    // Authoritative tile-occupancy index. Knows who is where and which tiles block movement.
    class Map
    {
    public:
        Map() = default;

        Map(std::uint32_t width, std::uint32_t height) :
                _width(width),
                _height(height)
        {}

        [[nodiscard]]
        bool isValid() const noexcept
        {
            return _width > 0 && _height > 0;
        }

        [[nodiscard]]
        std::uint32_t width() const noexcept
        {
            return _width;
        }

        [[nodiscard]]
        std::uint32_t height() const noexcept
        {
            return _height;
        }

        [[nodiscard]]
        bool isInside(Position p) const noexcept
        {
            return p.x < _width && p.y < _height;
        }

        [[nodiscard]]
        bool isTileBlocked(Position p) const noexcept
        {
            const auto it = _blockerByTile.find(pack(p));
            return it != _blockerByTile.end();
        }

        [[nodiscard]]
        bool isFreeForMove(Position p) const noexcept
        {
            return isInside(p) && !isTileBlocked(p);
        }

        [[nodiscard]]
        bool tryGetPosition(EntityId id, Position& out) const noexcept
        {
            const auto it = _positionById.find(id);
            if (it == _positionById.end())
            {
                return false;
            }
            out = it->second;
            return true;
        }

        void place(EntityId id, Position p, bool blocks)
        {
            _positionById[id] = p;
            if (blocks)
            {
                _blockerByTile[pack(p)] = id;
            }
        }

        void move(EntityId id, Position to, bool blocks)
        {
            if (auto it = _positionById.find(id); it != _positionById.end())
            {
                _blockerByTile.erase(pack(it->second));
            }
            _positionById[id] = to;
            if (blocks)
            {
                _blockerByTile[pack(to)] = id;
            }
        }

        void remove(EntityId id)
        {
            if (auto it = _positionById.find(id); it != _positionById.end())
            {
                _blockerByTile.erase(pack(it->second));
                _positionById.erase(it);
            }
        }

        // Chebyshev distance helper.
        [[nodiscard]]
        static std::uint32_t chebyshev(Position a, Position b) noexcept
        {
            const auto dx = a.x > b.x ? a.x - b.x : b.x - a.x;
            const auto dy = a.y > b.y ? a.y - b.y : b.y - a.y;
            return dx > dy ? dx : dy;
        }

        // Steps one tile toward `target` from `from`, choosing a free tile if possible.
        // Returns true if a step was taken.
        [[nodiscard]]
        bool tryStepToward(Position from, Position target, Position& out) const noexcept
        {
            if (from == target)
            {
                return false;
            }
            Position next = from;
            if (target.x < from.x)
            {
                next.x -= 1;
            }
            else if (target.x > from.x)
            {
                next.x += 1;
            }
            if (target.y < from.y)
            {
                next.y -= 1;
            }
            else if (target.y > from.y)
            {
                next.y += 1;
            }
            if (!isFreeForMove(next))
            {
                return false;
            }
            out = next;
            return true;
        }

        // Iterate all tracked entities (id, position).
        template <class Fn>
        void forEachEntity(Fn&& fn) const
        {
            for (const auto& [id, pos] : _positionById)
            {
                fn(id, pos);
            }
        }

    private:
        [[nodiscard]]
        std::uint64_t pack(Position p) const noexcept
        {
            return (static_cast<std::uint64_t>(p.x) << 32) | static_cast<std::uint64_t>(p.y);
        }

        std::uint32_t _width{0};
        std::uint32_t _height{0};
        std::unordered_map<EntityId, Position> _positionById;
        std::unordered_map<std::uint64_t, EntityId> _blockerByTile;
    };
}
