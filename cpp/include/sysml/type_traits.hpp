// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include <type_traits>

namespace sysml
{

template <class...>
struct all_are_same : std::true_type
{
};

template <class First, class Second, class... Rest>
struct all_are_same<First, Second, Rest...>
    : std::integral_constant<bool, std::is_same_v<First, Second> &&
                                       all_are_same<Second, Rest...>::value>
{
};

template <class... Ts>
constexpr bool all_are_same_v = all_are_same<Ts...>::value;

template <class...>
struct is_any_of : std::false_type
{
};

template <class First, class Second, class... Rest>
struct is_any_of<First, Second, Rest...>
    : std::integral_constant<bool, std::is_same_v<First, Second> ||
                                       is_any_of<First, Rest...>::value>
{
};

template <class... Ts>
constexpr bool is_any_of_v = is_any_of<Ts...>::value;

} // namespace sysml
