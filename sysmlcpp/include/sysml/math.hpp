// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/workaround/cpp20.hpp"

#include <type_traits>

namespace sysml
{

template <typename T>
constexpr inline T ceil_div(T a, std::type_identity_t<T> b) noexcept
{
    return (a + b - 1) / b;
}

template <typename T>
constexpr inline T round_up(T a, std::type_identity_t<T> b) noexcept
{
    return ceil_div(a, b) * b;
}

template <typename T>
constexpr inline std::tuple<T, T>
full_rest(T total, std::type_identity_t<T> delta) noexcept
{
    return {total / delta, total % delta};
}

// Equals to the number of iterations of the loop
// for (T i = from; i < to; i += stride)
// Assumes from <= to
template <typename T>
constexpr inline auto num_iterations(T from, std::type_identity_t<T> to,
                                     std::type_identity_t<T> stride) noexcept
    -> std::enable_if_t<std::is_integral_v<T>, T>
{
    return ceil_div(to - from, stride);
}

} // namespace sysml
