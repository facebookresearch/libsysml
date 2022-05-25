// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include <cstdint>
#include <type_traits>

namespace sysml
{

template <unsigned Size, bool IsSigned>
struct integer_type;

// clang-format off

template<> struct integer_type<1, false> { using type = std::uint8_t; };
template<> struct integer_type<1, true > { using type = std::int8_t ; };

template<> struct integer_type<2, false> { using type = std::uint16_t; };
template<> struct integer_type<2, true > { using type = std::int16_t ; };

template<> struct integer_type<4, false> { using type = std::uint32_t; };
template<> struct integer_type<4, true > { using type = std::int32_t ; };

template<> struct integer_type<8, false> { using type = std::uint64_t; };
template<> struct integer_type<8, true > { using type = std::int64_t ; };

// clang-format on

template <unsigned Size, bool IsSigned>
using integer_type_t = typename integer_type<Size, IsSigned>::type;

} // namespace sysml
