#include "World.hpp"

#include "IO/Events/MapCreated.hpp"
#include "IO/Events/MarchEnded.hpp"
#include "IO/Events/MarchStarted.hpp"
#include "IO/Events/UnitAbilityUsed.hpp"
#include "IO/Events/UnitAttacked.hpp"
#include "IO/Events/UnitDied.hpp"
#include "IO/Events/UnitMoved.hpp"
#include "IO/Events/UnitSpawned.hpp"
#include "IO/System/EventSystem.hpp"

#include <algorithm>

namespace sw::game
{
	World::World(EventSystem& events) :
			_map(0, 0),
			_events(events),
			_tick(0)
	{}

	void World::createMap(const uint32_t width, const uint32_t height)
	{
		_map = Map(width, height);
		_events.event(_tick, io::MapCreated{width, height});
	}

	void World::spawnUnit(
			const uint32_t unitId,
			const Position position,
			const uint32_t hp,
			const uint32_t strength,
			const uint32_t chance,
			const uint32_t rending)
	{
		_units.emplace_back(unitId, position, hp, strength, chance, rending);
		_map.move(unitId, position);
		_events.event(_tick, io::UnitSpawned{unitId, "swordsman", position.x, position.y});
	}

	void World::march(const uint32_t unitId, const Position targetPosition)
	{
		if (Unit* unit = getUnitById(unitId); unit != nullptr)
		{
			const Position currentPosition = unit->getCurrentPosition();
			unit->setTargetPosition(targetPosition);
			_events.event(
					_tick,
					io::MarchStarted{unitId, currentPosition.x, currentPosition.y, targetPosition.x, targetPosition.y});
		}
	}

	bool World::isGameOver() const
	{
		if (!_map.isValid())
		{
			return true;
		}
		return !std::ranges::any_of(_units, [](auto& unit) { return unit.isActive(); });
	}

	void World::step()
	{
		_tick++;
		for (auto& unit : _units)
		{
			if (!unit.isAlive())
			{
				continue;
			}

			const uint32_t unitId = unit.getId();
			const Position currentPosition = unit.getCurrentPosition();

			std::vector<uint32_t> targetUnitIds;
			_map.findTargets(unitId, currentPosition, targetUnitIds);

			bool idle = true;
			if (!targetUnitIds.empty())
			{
				for (const uint32_t targetUnitId : targetUnitIds)
				{
					if (Unit* target = getUnitById(targetUnitId); target != nullptr && target->isAlive())
					{
						if (unit.tryApplyAbility(*target))
						{
							_events.event(_tick, io::UnitAbilityUsed{unitId, "rending"});
						}
						else
						{
							unit.attack(*target);
							_events.event(
									_tick, io::UnitAttacked{unitId, targetUnitId, unit.getStrength(), target->getHp()});
						}

						if (!target->isAlive())
						{
							_map.remove(targetUnitId);
							_events.event(_tick, io::UnitDied{targetUnitId});
						}

						idle = false;
						break;
					}
				}
			}
			if (Position targetPosition; idle && unit.tryGetTargetPosition(targetPosition))
			{
				if (Position nextPosition;
					_map.tryGetNextPosition(unit.getCurrentPosition(), targetPosition, nextPosition))
				{
					unit.setCurrentPosition(nextPosition);
					_map.move(unitId, nextPosition);
					_events.event(_tick, io::UnitMoved{unitId, nextPosition.x, nextPosition.y});
				}
				if (Position newPosition = unit.getCurrentPosition(); newPosition == targetPosition)
				{
					unit.resetTargetPosition();
					_events.event(_tick, io::MarchEnded{unitId, newPosition.x, newPosition.y});
				}
				idle = false;
			}
			unit.setIdle(idle);
		}
	}

	Unit* World::getUnitById(const uint32_t unitId)
	{
		for (auto& unit : _units)
		{
			if (unit.getId() == unitId)
			{
				return &unit;
			}
		}
		return nullptr;
	}
}
