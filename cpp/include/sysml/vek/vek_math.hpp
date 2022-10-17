// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/vek/vek_struct.hpp"

namespace sysml
{

template <arithmetic T, std::size_t N, arithmetic U>
constexpr auto dot(vek<T, N> const& lhs, vek<U, N> const& rhs) noexcept
    -> decltype(std::declval<T>() * std::declval<U>())
{
    auto ret = lhs[0] * rhs[0];
    for (std::size_t i = 1; i < N; ++i)
        ret += lhs[i] * rhs[i];
    return ret;
}

} // namespace sysml
