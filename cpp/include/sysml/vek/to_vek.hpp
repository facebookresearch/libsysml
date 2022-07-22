// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/vek/vek_struct.hpp"

#include <array>
#include <type_traits> // for std::remove_cv, std::is_nothrow_convertibe, std::enable_if, std::is_convertible
#include <utility>     // for std::index_sequence

namespace sysml
{

namespace vek_detail
{

template <arithmetic Tp, std::size_t... Idx>
constexpr vek<Tp, sizeof...(Idx)>
vek_from_raw_helper(Tp const* a, std::index_sequence<Idx...>)
{
    return {{a[Idx]...}};
}

template <arithmetic Tp, std::size_t... Idx>
constexpr vek<Tp, sizeof...(Idx)> to_vek_helper(Tp (&a)[sizeof...(Idx)],
                                                std::index_sequence<Idx...>)
{
    return {{a[Idx]...}};
}

template <arithmetic Tp, std::size_t... Idx>
constexpr vek<Tp, sizeof...(Idx)>
to_vek_helper(std::array<Tp, sizeof...(Idx)> const& a,
              std::index_sequence<Idx...>)
{
    return {{a[Idx]...}};
}

template <arithmetic Tp, std::size_t... Idx>
constexpr std::array<Tp, sizeof...(Idx)>
vek_to_array_helper(vek<Tp, sizeof...(Idx)> const& a,
                    std::index_sequence<Idx...>)
{
    return {{a[Idx]...}};
}

} // namespace vek_detail

template <arithmetic Tp, std::size_t Nm>
constexpr vek<Tp, Nm>
to_vek(Tp (&a)[Nm]) noexcept(std::is_nothrow_constructible_v<Tp, Tp&>)
{
    static_assert(std::is_constructible_v<Tp, Tp&>);

    return vek_detail::to_vek_helper(a, std::make_index_sequence<Nm>{});
}

template <arithmetic Tp, std::size_t Nm>
constexpr vek<Tp, Nm>
to_vek(Tp(&&a)[Nm]) noexcept(std::is_nothrow_constructible_v<Tp, Tp&>)
{
    static_assert(std::is_constructible_v<Tp, Tp&>);

    return vek_detail::to_vek_helper(a, std::make_index_sequence<Nm>{});
}

template <arithmetic Tp, std::size_t Nm>
constexpr vek<Tp, Nm> to_vek(std::array<Tp, Nm> const& a) noexcept(
    std::is_nothrow_constructible_v<Tp, Tp&>)
{
    static_assert(std::is_constructible_v<Tp, Tp&>);

    return vek_detail::to_vek_helper(a, std::make_index_sequence<Nm>{});
}

template <std::size_t Nm, arithmetic Tp>
constexpr vek<Tp, Nm> vek_from_raw(Tp const* a)
{
    return vek_detail::vek_from_raw_helper(a, std::make_index_sequence<Nm>{});
}

template <arithmetic Tp, std::size_t Nm>
constexpr std::array<Tp, Nm> to_array(vek<Tp, Nm> const& a) noexcept
{
    return vek_detail::vek_to_array_helper(a, std::make_index_sequence<Nm>{});
}

} // namespace sysml
