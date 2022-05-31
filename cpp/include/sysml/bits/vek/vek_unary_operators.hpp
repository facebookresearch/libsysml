#pragma once

#include "sysml/bits/vek/vek_struct.hpp"

namespace sysml
{

namespace vek_detail
{

#define SYSML_VEK_DETAIL_DEFINE_VEK_UNARY_OPERATOR(OPER, NAME)                 \
    template <arithmetic T, std::size_t... Idx>                                \
    constexpr auto vek_unary_##NAME##_operator(                                \
        vek<T, sizeof...(Idx)> const& v, std::index_sequence<Idx...>)          \
        ->vek<decltype(OPER std::declval<T>()), sizeof...(Idx)>                \
    {                                                                          \
        return {{(OPER v[Idx])...}};                                           \
    }

SYSML_VEK_DETAIL_DEFINE_VEK_UNARY_OPERATOR(+, plus)
SYSML_VEK_DETAIL_DEFINE_VEK_UNARY_OPERATOR(-, minus)

#undef SYSML_VEK_DETAIL_DEFINE_VEK_UNARY_OPERATOR

} // namespace vek_detail

#define SYSML_VEK_DEFINE_VEK_UNARY_OPERATOR(OPER, NAME)                        \
    template <arithmetic T, std::size_t Nm>                                    \
    constexpr decltype(auto) operator OPER(vek<T, Nm> const& v)                \
    {                                                                          \
        return vek_detail::vek_unary_##NAME##_operator(                        \
            v, std::make_index_sequence<Nm>{});                                \
    }

SYSML_VEK_DEFINE_VEK_UNARY_OPERATOR(+, plus)
SYSML_VEK_DEFINE_VEK_UNARY_OPERATOR(-, minus)

#undef SYSML_VEK_DEFINE_VEK_UNARY_OPERATOR

} // namespace sysml
