#pragma once

namespace sw::game::event
{
	struct RoundStartTick
	{
	};

	struct RoundEndTick
	{};

	struct TurnStartTick
	{
		core::EntityId currentTurnEntityId;
	};

	struct TurnEndTick
	{
		core::EntityId currentTurnEntityId;
	};

	struct ActionTick
	{
	};

	template <class T>
	struct Add
	{
	};

	template <class T>
	struct AddExisting
	{
		T& component;
	};

	template <class T>
	struct Remove
	{
	};

	struct Spawn
	{
	};

	struct Die
	{
	};
}
