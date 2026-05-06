#pragma once

namespace sw::core
{
    // Extract owner class and signature parts from a pointer-to-member-function.
    template <class T>
    struct MethodTraits;

    template <class C, class R, class... A>
    struct MethodTraits<R (C::*)(A...)>
    {
        using class_type = C;
        using return_type = R;
    };

    template <class C, class R, class... A>
    struct MethodTraits<R (C::*)(A...) const>
    {
        using class_type = C;
        using return_type = R;
    };
}
