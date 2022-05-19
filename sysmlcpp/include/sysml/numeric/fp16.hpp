// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "bits/fp16/detect.hpp"

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

#    include "bits/fp16/half.hpp"

#    define SYSML_FLOAT16_TYPE half_float::half

#endif

namespace sysml
{

using fp16 = SYSML_FLOAT16_TYPE;
static_assert(sizeof(fp16) == 2);

} // namespace sysml

#undef SYSML_FLOAT16_TYPE
