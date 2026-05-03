#pragma once
#include "Event.hpp"
#include "Game/Core/Entity.hpp"
#include "Game/Core/Component/ComponentsContainer.hpp"
#include "EventTypeRegistry.hpp"

#include <algorithm>
#include <array>
#include <random>
#include <unordered_map>

namespace sw
{
	class EventSystem;
}

namespace sw::core
{
	class EventHandlerSystem;
	class EventHandlerRegistry;
	class CommandBuffer;
	class Map;
	class Balance;

	struct EventContext
	{
		CommandBuffer& commands;
		const ComponentsContainer& components;
		EventSystem& eventSystem;
		const EventHandlerSystem& eventRegistry;
		Map& map;
		std::mt19937& random;
		std::uint64_t tick{};
	};

	namespace detail
	{
		using HandlerFn = void(*)();
		using TrampolineFn = void(*)(HandlerFn, const EventContext&, EntityId, void*);

		struct EventHandler
		{
			TrampolineFn trampoline;
			HandlerFn handler;
		};

		template <class T, typename... Args>
		void eventHandlerTrampoline(HandlerFn handler, const EventContext& context, EntityId entityId, void* event)
		{
			using TypedHandler = void(*)(T&, const EventContext&, EntityId, Args&...);

			std::array<void*, sizeof...(Args)> args
					= {context.components.getComponent<std::remove_cvref_t<Args>>(entityId)...};

			if (!std::ranges::all_of(
					args,
					[](const void* const arg)
					{
						return arg != nullptr;
					}))
			{
				return;
			}

			[]<size_t... I>(
					TypedHandler typed_handler,
					const EventContext& context,
					EntityId entityId,
					void* event,
					std::array<void*, sizeof...(Args)>& args,
					std::index_sequence<I...>)
					{
						typed_handler(*static_cast<T*>(event), context, entityId, *static_cast<Args*>(args[I])...);
					}(reinterpret_cast<TypedHandler>(handler),
						context,
						entityId,
						event,
						args,
						std::make_index_sequence<sizeof...(Args)>());
		}
	}

	class EventHandlerSystem
	{
	public:
		EventHandlerSystem() = default;

		template <class T>
		void emit(T&& event, const EventContext& context, EntityId entityId) const
		{
			const auto it = _handlers.find(EventTypeRegistry::getId<std::remove_cvref_t<T>>());
			if (it != _handlers.end())
			{
				for (const auto& handler : it->second)
				{
					handler.trampoline(handler.handler, context, entityId, &event);
				}
			}
		}

	private:
		friend class EventHandlerRegistry;

		explicit EventHandlerSystem(std::unordered_map<EventId, std::vector<detail::EventHandler>> handlers)
			: _handlers(std::move(handlers))
		{
		}

		std::unordered_map<EventId, std::vector<detail::EventHandler>> _handlers;
	};

	class EventHandlerRegistry
	{
	public:
		template <class T, typename... Args>
		void add(void (*handler)(T&, const EventContext&, EntityId entityId, Args&...))
		{
			_handlers[EventTypeRegistry::getId<std::remove_cvref_t<T>>()].emplace_back(
					detail::EventHandler{
							.trampoline = &detail::eventHandlerTrampoline<T, Args...>,
							.handler = reinterpret_cast<detail::HandlerFn>(handler)
					});
		}

		EventHandlerSystem createEventHandlerSystem() &&
		{
			return EventHandlerSystem{std::move(_handlers)};
		}

	private:
		std::unordered_map<EventId, std::vector<detail::EventHandler>> _handlers;
	};
}
