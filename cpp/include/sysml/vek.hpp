#pragma once

#include "sysml/bits/vek/std_inject.hpp"
#include "sysml/bits/vek/to_vek.hpp"
#include "sysml/bits/vek/vek_binary_operators.hpp"
#include "sysml/bits/vek/vek_cast.hpp"
#include "sysml/bits/vek/vek_comparisons.hpp"
#include "sysml/bits/vek/vek_struct.hpp"
#include "sysml/bits/vek/vek_unary_operators.hpp"

#include "sysml/numeric.hpp"

#include "sysml/bits/vek/is_vek.hpp"
#include "sysml/bits/vek_helpers.hpp"

inline int add2(int a, int b) noexcept { return a + b; }

#include <algorithm> // for std::fill_n, std::swap, std::swap_ranges, std::equal, std::lexicographical_compare
#include <cstddef>   // for std::size_t
#include <iterator>  // for std::reverse_iterator
#include <stdexcept> // for std::out_of_range
#include <type_traits> // for std::is_nothrow_swappable, std::enable_if, std::is_same

namespace sysml
{

// Vek arithmetics

#if 0

#    define SYSML_VEK_BINARY_OPERATOR_DEFINITION(OPER, NAME)                   \
        template <typename LTp, std::size_t Nm, typename RTp>                  \
        constexpr inline auto operator OPER(vek<LTp, Nm> const& lhs,           \
                                            RTp const&          rhs)           \
            ->std::enable_if_t<                                                \
                vek_depth_v<LTp> == vek_depth_v<RTp> &&                        \
                    helpers::is_##NAME##_operable_from_v<LTp, RTp>,            \
                vek<helpers::result_of_##NAME##_from_t<LTp, RTp>, Nm>>         \
        {                                                                      \
            vek<helpers::result_of_##NAME##_from_t<LTp, RTp>, Nm> ret;         \
            for (std::size_t i = 0; i < Nm; ++i)                               \
            {                                                                  \
                ret[i] = lhs[i] OPER rhs;                                      \
            }                                                                  \
            return ret;                                                        \
        }                                                                      \
                                                                               \
        template <typename LTp, typename RTp, std::size_t Nm>                  \
        constexpr inline auto operator OPER(LTp const&          lhs,           \
                                            vek<RTp, Nm> const& rhs)           \
            ->std::enable_if_t<                                                \
                vek_depth_v<LTp> == vek_depth_v<RTp> &&                        \
                    helpers::is_##NAME##_operable_from_v<LTp, RTp>,            \
                vek<helpers::result_of_##NAME##_from_t<LTp, RTp>, Nm>>         \
        {                                                                      \
            vek<helpers::result_of_##NAME##_from_t<LTp, RTp>, Nm> ret;         \
            for (std::size_t i = 0; i < Nm; ++i)                               \
            {                                                                  \
                ret[i] = lhs OPER rhs[i];                                      \
            }                                                                  \
            return ret;                                                        \
        }                                                                      \
                                                                               \
        template <typename LTp, typename RTp, std::size_t Nm>                  \
        constexpr inline auto operator OPER(vek<LTp, Nm> const& lhs,           \
                                            vek<RTp, Nm> const& rhs)           \
            ->std::enable_if_t<                                                \
                vek_depth_v<LTp> == vek_depth_v<RTp> &&                        \
                    helpers::is_##NAME##_operable_from_v<LTp, RTp>,            \
                vek<helpers::result_of_##NAME##_from_t<LTp, RTp>, Nm>>         \
        {                                                                      \
            vek<helpers::result_of_##NAME##_from_t<LTp, RTp>, Nm> ret;         \
            for (std::size_t i = 0; i < Nm; ++i)                               \
            {                                                                  \
                ret[i] = lhs[i] OPER rhs[i];                                   \
            }                                                                  \
            return ret;                                                        \
        }

SYSML_VEK_BINARY_OPERATOR_DEFINITION(+, plus)

#    undef SYSML_VEK_BINARY_OPERATOR_DEFINITION

#endif

template <std::size_t Idx, typename Tp, std::size_t Nm>
constexpr Tp& get(sysml::vek<Tp, Nm>& v) noexcept
{
    static_assert(Idx < Nm);
    return v[Idx];
}

template <std::size_t Idx, typename Tp, std::size_t Nm>
constexpr Tp&& get(sysml::vek<Tp, Nm>&& v) noexcept
{
    static_assert(Idx < Nm);
    return std::move(get<Idx>(v));
}

template <std::size_t Idx, typename Tp, std::size_t Nm>
constexpr Tp const& get(sysml::vek<Tp, Nm> const& v) noexcept
{
    static_assert(Idx < Nm);
    return v[Idx];
}

template <std::size_t Idx, typename Tp, std::size_t Nm>
constexpr Tp const&& get(sysml::vek<Tp, Nm> const&& v) noexcept
{
    static_assert(Idx < Nm);
    return std::move(get<Idx>(v));
}

} // namespace sysml
