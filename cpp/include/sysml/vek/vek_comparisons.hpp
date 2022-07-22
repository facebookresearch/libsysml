// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/vek/vek_struct.hpp"

#include <algorithm>   // std::equal, std::lexicographical_compare
#include <cstddef>     // for std::size_t
#include <type_traits> // for std::false_type, std::void_t
#include <utility>     // for std::declval

namespace sysml
{

namespace vek_detail
{

template <class, class, class = void>
struct is_equal_comparable : std::false_type
{
};
template <class A, class B>
struct is_equal_comparable<
    A, B, std::void_t<decltype(std::declval<A>() == std::declval<B>())>>
    : std::true_type
{
};

template <class A, class B>
constexpr bool is_equal_comparable_v = is_equal_comparable<A, B>::value;

template <class, class, class = void>
struct is_lt_comparable : std::false_type
{
};
template <class A, class B>
struct is_lt_comparable<
    A, B, std::void_t<decltype(std::declval<A>() < std::declval<B>())>>
    : std::true_type
{
};

template <class A, class B>
constexpr bool is_lt_comparable_v = is_equal_comparable<A, B>::value;

} // namespace vek_detail

// Vek comparisons

template <typename TpL, std::size_t Nm, typename TpR>
constexpr inline auto operator==(vek<TpL, Nm> const& lhs,
                                 vek<TpR, Nm> const& rhs)
    -> std::enable_if_t<vek_detail::is_equal_comparable_v<TpL, TpR>, bool>
{
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename TpL, std::size_t Nm, typename TpR>
constexpr inline auto operator!=(vek<TpL, Nm> const& lhs,
                                 vek<TpR, Nm> const& rhs)
    -> std::enable_if_t<vek_detail::is_equal_comparable_v<TpL, TpR>, bool>
{
    return !(lhs == rhs);
}

template <typename TpL, std::size_t Nm, typename TpR>
constexpr inline auto operator<(vek<TpL, Nm> const& lhs,
                                vek<TpR, Nm> const& rhs)
    -> std::enable_if_t<vek_detail::is_lt_comparable_v<TpL, TpR>, bool>
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
                                        rhs.end());
}

template <typename TpL, std::size_t Nm, typename TpR>
constexpr inline auto operator>(vek<TpL, Nm> const& lhs,
                                vek<TpR, Nm> const& rhs)
    -> std::enable_if_t<vek_detail::is_lt_comparable_v<TpR, TpL>, bool>
{
    return (rhs < lhs);
}

template <typename TpL, std::size_t Nm, typename TpR>
constexpr inline auto operator<=(vek<TpL, Nm> const& lhs,
                                 vek<TpR, Nm> const& rhs)
    -> std::enable_if_t<vek_detail::is_lt_comparable_v<TpR, TpL>, bool>
{
    return !(rhs < lhs);
}

template <typename TpL, std::size_t Nm, typename TpR>
constexpr inline auto operator>=(vek<TpL, Nm> const& lhs,
                                 vek<TpR, Nm> const& rhs)
    -> std::enable_if_t<vek_detail::is_lt_comparable_v<TpL, TpR>, bool>
{
    return !(lhs < rhs);
}

} // namespace sysml
