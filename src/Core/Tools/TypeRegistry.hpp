#pragma once

#include <concepts>
#include <type_traits>

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
            static const auto id = T(_nextId++);
            return id;
        }

    private:
        inline static typename T::underlying_type _nextId{};
    };
}
