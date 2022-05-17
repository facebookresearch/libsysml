// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include <type_traits>

namespace sysml
{

struct zi
{
};

template <typename T>
struct is_arithmetic : std::is_arithmetic<T>
{
};

template <typename T>
constexpr bool is_arithmetic_v = is_arithmetic<T>::value;

template <typename T>
concept arithmetic = is_arithmetic_v<T>;

template <typename T>
struct is_integral : std::is_integral<T>
{
};

template <typename T>
constexpr bool is_integral_v = is_integral<T>::value;

template <typename T>
concept integral = is_integral_v<T>;

template <typename T>
struct is_floating_point : std::is_floating_point<T>
{
};

template <typename T>
constexpr bool is_floating_point_v = is_floating_point<T>::value;

template <typename T>
concept floating_point = is_floating_point_v<T>;

} // namespace sysml
