// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/bits/numeric/fp16/detect.hpp"

#if defined(SYSML_HAS___Float16)

#    include <ostream>

template <class CharT, class Traits>
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, _Float16 v)
{
    os << static_cast<float>(v);
    return os;
}

#    define SYSML_FLOAT16_TYPE _Float16

#elif defined(SYSML_HAS___fp16)

#    define SYSML_FLOAT16_TYPE __fp16

#else

#    include "sysml/bits/numeric/fp16/half.hpp"

#    define SYSML_FLOAT16_TYPE half_float::half

#endif

#include <type_traits>

namespace sysml
{

using fp16_t = SYSML_FLOAT16_TYPE;
static_assert(sizeof(fp16_t) == 2);

template <typename>
struct is_arithmetic;

template <>
struct is_arithmetic<fp16_t> : std::true_type
{
};

template <typename>
struct is_floating_point;

template <>
struct is_floating_point<fp16_t> : std::true_type
{
};

} // namespace sysml

#undef SYSML_FLOAT16_TYPE
