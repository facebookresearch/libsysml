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

} // namespace sysml
