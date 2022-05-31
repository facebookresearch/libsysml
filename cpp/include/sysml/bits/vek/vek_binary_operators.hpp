#pragma once

#include "sysml/bits/vek/vek_struct.hpp"

namespace sysml
{

namespace vek_detail
{

#define SYSML_VEK_DETAIL_DEFINE_VEK_BINARY_OPERATOR(OPER, NAME)                \
    template <arithmetic T, arithmetic U, std::size_t... Idx>                  \
    constexpr auto vek_##NAME##_value(vek<T, sizeof...(Idx)> const& lhs,       \
                                      U rhs, std::index_sequence<Idx...>)      \
        ->vek<decltype(std::declval<T>() OPER std::declval<U>()),              \
              sizeof...(Idx)>                                                  \
    {                                                                          \
        return {{(lhs[Idx] OPER rhs)...}};                                     \
    }                                                                          \
                                                                               \
    template <arithmetic T, arithmetic U, std::size_t... Idx>                  \
    constexpr auto value_##NAME##_vek(                                         \
        T lhs, vek<U, sizeof...(Idx)> const& rhs, std::index_sequence<Idx...>) \
        ->vek<decltype(std::declval<T>() OPER std::declval<U>()),              \
              sizeof...(Idx)>                                                  \
    {                                                                          \
        return {{(lhs OPER rhs[Idx])...}};                                     \
    }                                                                          \
                                                                               \
    template <arithmetic T, arithmetic U, std::size_t... Idx>                  \
    constexpr auto vek_##NAME##_vek(vek<T, sizeof...(Idx)> const& lhs,         \
                                    vek<U, sizeof...(Idx)> const& rhs,         \
                                    std::index_sequence<Idx...>)               \
        ->vek<decltype(std::declval<T>() OPER std::declval<U>()),              \
              sizeof...(Idx)>                                                  \
    {                                                                          \
        return {{(lhs[Idx] OPER rhs[Idx])...}};                                \
    }

SYSML_VEK_DETAIL_DEFINE_VEK_BINARY_OPERATOR(+, plus)
SYSML_VEK_DETAIL_DEFINE_VEK_BINARY_OPERATOR(-, minus)
SYSML_VEK_DETAIL_DEFINE_VEK_BINARY_OPERATOR(*, times)
SYSML_VEK_DETAIL_DEFINE_VEK_BINARY_OPERATOR(/, div)
SYSML_VEK_DETAIL_DEFINE_VEK_BINARY_OPERATOR(%, mod)
SYSML_VEK_DETAIL_DEFINE_VEK_BINARY_OPERATOR(&, binary_and)
SYSML_VEK_DETAIL_DEFINE_VEK_BINARY_OPERATOR(|, binary_or)
SYSML_VEK_DETAIL_DEFINE_VEK_BINARY_OPERATOR(^, binary_xor)
SYSML_VEK_DETAIL_DEFINE_VEK_BINARY_OPERATOR(<<, binary_shl)
SYSML_VEK_DETAIL_DEFINE_VEK_BINARY_OPERATOR(>>, binary_shr)

#undef SYSML_VEK_DETAIL_DEFINE_VEK_BINARY_OPERATOR

} // namespace vek_detail

#define SYSML_VEK_DEFINE_VEK_BINARY_OPERATOR(OPER, NAME)                       \
    template <arithmetic T, std::size_t Nm, arithmetic U>                      \
    constexpr decltype(auto) operator OPER(vek<T, Nm> const& lhs, U rhs)       \
    {                                                                          \
        return vek_detail::vek_##NAME##_value(lhs, rhs,                        \
                                              std::make_index_sequence<Nm>{}); \
    }                                                                          \
                                                                               \
    template <arithmetic T, arithmetic U, std::size_t Nm>                      \
    constexpr decltype(auto) operator OPER(T lhs, vek<U, Nm> const& rhs)       \
    {                                                                          \
        return vek_detail::value_##NAME##_vek(lhs, rhs,                        \
                                              std::make_index_sequence<Nm>{}); \
    }                                                                          \
                                                                               \
    template <arithmetic T, arithmetic U, std::size_t Nm>                      \
    constexpr decltype(auto) operator OPER(vek<T, Nm> const& lhs,              \
                                           vek<U, Nm> const& rhs)              \
    {                                                                          \
        return vek_detail::vek_##NAME##_vek(lhs, rhs,                          \
                                            std::make_index_sequence<Nm>{});   \
    }

SYSML_VEK_DEFINE_VEK_BINARY_OPERATOR(+, plus)
SYSML_VEK_DEFINE_VEK_BINARY_OPERATOR(-, minus)
SYSML_VEK_DEFINE_VEK_BINARY_OPERATOR(*, times)
SYSML_VEK_DEFINE_VEK_BINARY_OPERATOR(/, div)
SYSML_VEK_DEFINE_VEK_BINARY_OPERATOR(%, mod)
SYSML_VEK_DEFINE_VEK_BINARY_OPERATOR(&, binary_and)
SYSML_VEK_DEFINE_VEK_BINARY_OPERATOR(|, binary_or)
SYSML_VEK_DEFINE_VEK_BINARY_OPERATOR(^, binary_xor)
SYSML_VEK_DEFINE_VEK_BINARY_OPERATOR(<<, binary_shl)
SYSML_VEK_DEFINE_VEK_BINARY_OPERATOR(>>, binary_shr)

#undef SYSML_VEK_DEFINE_VEK_BINARY_OPERATOR

} // namespace sysml
