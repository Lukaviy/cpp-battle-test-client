#pragma once

namespace sw::core
{
    // Type-level list of interfaces. Cheaper to compile than std::tuple,
    // because it carries no value-semantics machinery.
    template <class... I>
    struct InterfaceList
    {
        static constexpr unsigned size = sizeof...(I);
    };
}
