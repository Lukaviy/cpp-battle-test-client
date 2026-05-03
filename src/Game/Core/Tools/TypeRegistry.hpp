#pragma once
#include <concepts>

namespace sw::core
{
	template <class T>
		requires requires { typename T::underlying_type; } && std::integral<typename T::underlying_type>
	class TypeRegistry
	{
	public:
		template <class C>
			requires(!std::is_pointer_v<C> && !std::is_reference_v<C> && std::is_same_v<C, std::remove_cv_t<C>>)
		static T getId()
		{
			static auto id = T(_nextId++);
			return id;
		}

	private:
		inline static T::underlying_type _nextId{};
	};
}
