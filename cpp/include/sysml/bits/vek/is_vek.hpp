// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/bits/arithmetic.hpp"

#include <type_traits>

namespace sysml
{

template <arithmetic, std::size_t>
struct vek;

template <class>
struct is_vek : std::false_type
{
};

template <arithmetic Tp, std::size_t Nm>
struct is_vek<vek<Tp, Nm>> : std::true_type
{
};

template <class T>
constexpr bool is_vek_v = is_vek<T>::value;

} // namespace sysml
