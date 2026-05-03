#pragma once

#include <any>
#include <memory>
#include <span>
#include <unordered_map>
#include <unordered_set>
#include <Game/Core/Entity.hpp>
#include <Game/Core/Component/Component.hpp>
#include <Game/Core/Component/ComponentTypeRegistry.hpp>

namespace sw::core
{
	class ComponentsContainer
	{
	public:
		template <class T>
		T* getComponent(EntityId entity_id) const
		{
			return static_cast<T*>(getComponent(entity_id, ComponentTypeRegistry::getId<T>()));
		}

		template <class T>
		std::remove_cvref_t<T>& addComponent(EntityId entity_id, T&& component)
		{
			using C = std::remove_cvref_t<T>;
			const auto component_id = ComponentTypeRegistry::getId<C>();
			auto it = _componentArrays.find(component_id);

			if (it == _componentArrays.end())
			{
				it = _componentArrays.emplace(component_id, std::make_unique<ComponentsArray<C>>()).first;
			}

			return *static_cast<C*>(it->second->addComponent(entity_id, &component));
		}

		template <class T>
		void removeComponent(EntityId entity_id)
		{
			removeComponent(entity_id, ComponentTypeRegistry::getId<T>());
		}

		void removeComponents(EntityId entity_id);

		template <class T>
			requires (!std::is_empty_v<T>)
		std::span<T> getComponents() const
		{
			const auto component_id = ComponentTypeRegistry::getId<T>();
			const auto it = _componentArrays.find(component_id);
			if (it == _componentArrays.end())
			{
				return {};
			}
			return static_cast<ComponentsArray<T>*>(it->second.get())->getComponents();
		}

	private:
		std::unordered_map<EntityId, std::vector<ComponentId>> _entityComponents;

		void* getComponent(EntityId entity_id, ComponentId component_id) const;
		void removeComponent(EntityId entity_id, ComponentId component_id);

		class ComponentsArrayBase
		{
		public:
			virtual ~ComponentsArrayBase() = default;

			virtual void* getComponent(EntityId entity_id) = 0;
			virtual void removeComponent(EntityId entity_id) = 0;
			virtual void* addComponent(EntityId entity_id, void* component) = 0;
		};

		template <class T>
		class ComponentsArray;

		template <class T>
			requires (!std::is_empty_v<T>)
		class ComponentsArray<T> : public ComponentsArrayBase
		{
		public:
			void* getComponent(EntityId entity_id) override
			{
				const auto it = _entityToIndex.find(entity_id);
				if (it == _entityToIndex.end())
				{
					return nullptr;
				}
				return &_components[it->second];
			}

			void removeComponent(EntityId entity_id) override
			{
				const auto it = _entityToIndex.find(entity_id);
				if (it == _entityToIndex.end())
				{
					return;
				}
				std::swap(_components[it->second], _components.back());
				std::swap(_indexToEntity[it->second], _indexToEntity.back());
				_entityToIndex[_indexToEntity[it->second]] = it->second;
				_components.pop_back();
				_indexToEntity.pop_back();
				_entityToIndex.erase(it);
			}

			void* addComponent(EntityId entity_id, void* component) override
			{
				const auto it = _entityToIndex.find(entity_id);
				auto* const typed_component = static_cast<T*>(component);
				if (it != _entityToIndex.end())
				{
					_components[it->second] = std::move(*typed_component);
					return &_components[it->second];
				}
				_components.emplace_back(std::move(*typed_component));
				_indexToEntity.push_back(entity_id);
				_entityToIndex[entity_id] = _components.size() - 1;
				return &_components.back();
			}

			std::span<T> getComponents()
			{
				return _components;
			}

		private:
			std::unordered_map<EntityId, size_t> _entityToIndex;
			std::vector<T> _components;
			std::vector<EntityId> _indexToEntity;
		};

		template <class T>
			requires (std::is_empty_v<T>)
		class ComponentsArray<T> : public ComponentsArrayBase
		{
		public:
			void* getComponent(EntityId entity_id) override
			{
				const auto it = _hasEntity.find(entity_id);
				if (it == _hasEntity.end())
				{
					return nullptr;
				}
				return &_dummy;
			}

			void removeComponent(EntityId entity_id) override
			{
				const auto it = _hasEntity.find(entity_id);
				if (it == _hasEntity.end())
				{
					return;
				}
				_hasEntity.erase(it);
			}

			void* addComponent(EntityId entity_id, void* component) override
			{
				const auto it = _hasEntity.find(entity_id);
				if (it != _hasEntity.end())
				{
					return &_dummy;
				}
				_hasEntity.insert(entity_id);
				return &_dummy;
			}

		private:
			inline static T _dummy;
			std::unordered_set<EntityId> _hasEntity;
		};

		std::unordered_map<ComponentId, std::unique_ptr<ComponentsArrayBase>> _componentArrays;
	};
}
