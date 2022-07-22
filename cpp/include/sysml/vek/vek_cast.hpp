// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/vek/vek_struct.hpp"

#include <cstddef>     // for std::size_t
#include <type_traits> // for std::is_nothrow_convertible
#include <utility>     // std::index_sequence

namespace sysml
{

namespace vek_detail
{

template <typename ToTp, typename FromTp, std::size_t... Idx>
constexpr vek<ToTp, sizeof...(Idx)>
vek_cast_helper(vek<FromTp, sizeof...(Idx)> const& a,
                std::index_sequence<Idx...>)
{
    return {{static_cast<ToTp>(a[Idx])...}};
}

} // namespace vek_detail

template <typename ToTp, typename FromTp, std::size_t Nm>
constexpr auto vek_cast(vek<FromTp, Nm> const& a) noexcept(
    std::is_nothrow_convertible_v<ToTp, FromTp>)
    -> std::enable_if_t<std::is_convertible_v<ToTp, FromTp>, vek<ToTp, Nm>>
{
    return vek_detail::vek_cast_helper<ToTp>(a, std::make_index_sequence<Nm>{});
}

} // namespace sysml
