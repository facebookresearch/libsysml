// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include <type_traits>

namespace sysml
{

template <typename...>
struct all_are_same : std::true_type
{
};

template <typename First, typename Second, typename... Rest>
struct all_are_same<First, Second, Rest...>
    : std::integral_constant<bool, std::is_same_v<First, Second> &&
                                       all_are_same<Second, Rest...>::value>
{
};

template <typename... Ts>
constexpr bool all_are_same_v = all_are_same<Ts...>::value;

} // namespace sysml
