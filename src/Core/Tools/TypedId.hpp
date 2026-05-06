#pragma once

#include <compare>
#include <type_traits>

namespace sw::core
{
    template <class C, class T>
    class TypedId
    {
    public:
        using underlying_type = T;

        constexpr TypedId() = default;

        constexpr explicit TypedId(T value) :
                _value(value)
        {}

        constexpr const T& get_raw() const noexcept
        {
            return _value;
        }

        constexpr auto operator<=>(const TypedId&) const = default;

    private:
        T _value{};
    };
}

template <class C, class T>
struct std::hash<sw::core::TypedId<C, T>>
{
    std::size_t operator()(const sw::core::TypedId<C, T>& id) const noexcept
    {
        return std::hash<T>{}(id.get_raw());
    }
};
