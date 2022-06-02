#pragma once

#include "sysml/numeric.hpp"
#include <type_traits> // for std::is_same, std::void_t
#include <utility>     // for std::declval

namespace sysml::helpers
{

#define SYSML_VEK_HELPERS_IS_COMPOUND_ASSIGNABLE_DEFINER(OPER, NAME)           \
    template <class, class, class = void>                                      \
    struct is_##NAME##_compound_assignable : std::false_type                   \
    {                                                                          \
    };                                                                         \
    template <class A, class B>                                                \
    struct is_##NAME##_compound_assignable<                                    \
        A, B,                                                                  \
        std::void_t<std::is_same<                                              \
            A&, decltype(std::declval<A&>() OPER std::declval<B const&>())>>>  \
        : std::true_type                                                       \
    {                                                                          \
    };                                                                         \
    template <class A, class B>                                                \
    constexpr bool is_##NAME##_compound_assignable_v =                         \
        is_##NAME##_compound_assignable<A, B>::value

SYSML_VEK_HELPERS_IS_COMPOUND_ASSIGNABLE_DEFINER(+=, plus_equal);
SYSML_VEK_HELPERS_IS_COMPOUND_ASSIGNABLE_DEFINER(-=, minus_equal);
SYSML_VEK_HELPERS_IS_COMPOUND_ASSIGNABLE_DEFINER(*=, times_equal);
SYSML_VEK_HELPERS_IS_COMPOUND_ASSIGNABLE_DEFINER(/=, div_equal);
SYSML_VEK_HELPERS_IS_COMPOUND_ASSIGNABLE_DEFINER(%=, mod_equal);
SYSML_VEK_HELPERS_IS_COMPOUND_ASSIGNABLE_DEFINER(&=, band_equal);
SYSML_VEK_HELPERS_IS_COMPOUND_ASSIGNABLE_DEFINER(|=, bor_equal);
SYSML_VEK_HELPERS_IS_COMPOUND_ASSIGNABLE_DEFINER(^=, bxor_equal);
SYSML_VEK_HELPERS_IS_COMPOUND_ASSIGNABLE_DEFINER(<<=, bshl_equal);
SYSML_VEK_HELPERS_IS_COMPOUND_ASSIGNABLE_DEFINER(>>=, bshr_equal);

#undef SYSML_VEK_HELPERS_IS_COMPOUND_ASSIGNABLE_DEFINER

#define SYSML_VEK_HELPERS_OPERATOR_DEFINER(OPER, NAME)                         \
    template <class, class, class = void>                                      \
    struct is_##NAME##_operable_from : std::false_type                         \
    {                                                                          \
    };                                                                         \
    template <class A, class B>                                                \
    struct is_##NAME##_operable_from<                                          \
        A, B,                                                                  \
        std::void_t<decltype(std::declval<A const&>()                          \
                                 OPER std::declval<B const&>())>>              \
        : std::true_type                                                       \
    {                                                                          \
    };                                                                         \
    template <class A, class B>                                                \
    constexpr bool is_##NAME##_operable_from_v =                               \
        is_##NAME##_operable_from<A, B>::value;                                \
    template <class A, class B>                                                \
    struct result_of_##NAME##_from                                             \
    {                                                                          \
        using type =                                                           \
            decltype(std::declval<A const&>() OPER std::declval<B const&>());  \
    };                                                                         \
    template <class A, class B>                                                \
    using result_of_##NAME##_from_t =                                          \
        typename result_of_##NAME##_from<A, B>::type

SYSML_VEK_HELPERS_OPERATOR_DEFINER(+, plus);
SYSML_VEK_HELPERS_OPERATOR_DEFINER(-, minus);
SYSML_VEK_HELPERS_OPERATOR_DEFINER(*, times);
SYSML_VEK_HELPERS_OPERATOR_DEFINER(/, div);
SYSML_VEK_HELPERS_OPERATOR_DEFINER(%, mod);
SYSML_VEK_HELPERS_OPERATOR_DEFINER(&, band);
SYSML_VEK_HELPERS_OPERATOR_DEFINER(|, bor);
SYSML_VEK_HELPERS_OPERATOR_DEFINER(^, bxor);
SYSML_VEK_HELPERS_OPERATOR_DEFINER(&&, land);
SYSML_VEK_HELPERS_OPERATOR_DEFINER(||, lor);
SYSML_VEK_HELPERS_OPERATOR_DEFINER(<<, bshl);
SYSML_VEK_HELPERS_OPERATOR_DEFINER(>>, bshr);

// template <class, class, class = void>
// struct is_plus_operable_from : std::false_type
// {
// };
// template <class A, class B>
// struct is_plus_operable_from<
//     A, B,
//     std::void_t<decltype(std::declval<A const&>() + std::declval<B
//     const&>())>> : std::true_type
// {
// };
// template <class A, class B>
// constexpr bool is_plus_operable_from_v = is_plus_operable_from<A, B>::value;

#undef SYSML_VEK_HELPERS_OPERATOR_DEFINER

} // namespace sysml::helpers
