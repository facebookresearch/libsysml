// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/vek/vek_struct.hpp"

#include <cstddef>
#include <cstdint>

namespace sysml
{

#define SYSML_VEK_DETAIL_DEFINE_VEK_COMMON_TYPE(suffix, type)                  \
    using vek1##suffix  = vek<type, 1>;                                        \
    using vek2##suffix  = vek<type, 2>;                                        \
    using vek3##suffix  = vek<type, 3>;                                        \
    using vek4##suffix  = vek<type, 4>;                                        \
    using vek5##suffix  = vek<type, 5>;                                        \
    using vek6##suffix  = vek<type, 6>;                                        \
    using vek7##suffix  = vek<type, 7>;                                        \
    using vek8##suffix  = vek<type, 8>;                                        \
    using vek9##suffix  = vek<type, 9>;                                        \
    using vek10##suffix = vek<type, 10>

SYSML_VEK_DETAIL_DEFINE_VEK_COMMON_TYPE(i, int);
SYSML_VEK_DETAIL_DEFINE_VEK_COMMON_TYPE(l, long);
SYSML_VEK_DETAIL_DEFINE_VEK_COMMON_TYPE(ll, long long);
SYSML_VEK_DETAIL_DEFINE_VEK_COMMON_TYPE(u, unsigned);
SYSML_VEK_DETAIL_DEFINE_VEK_COMMON_TYPE(ul, unsigned long);
SYSML_VEK_DETAIL_DEFINE_VEK_COMMON_TYPE(ull, unsigned long long);
SYSML_VEK_DETAIL_DEFINE_VEK_COMMON_TYPE(size, std::size_t);
SYSML_VEK_DETAIL_DEFINE_VEK_COMMON_TYPE(ssize, std::ptrdiff_t);

SYSML_VEK_DETAIL_DEFINE_VEK_COMMON_TYPE(i8, std::int8_t);
SYSML_VEK_DETAIL_DEFINE_VEK_COMMON_TYPE(i16, std::int16_t);
SYSML_VEK_DETAIL_DEFINE_VEK_COMMON_TYPE(i32, std::int32_t);
SYSML_VEK_DETAIL_DEFINE_VEK_COMMON_TYPE(i64, std::int64_t);

SYSML_VEK_DETAIL_DEFINE_VEK_COMMON_TYPE(ui8, std::uint8_t);
SYSML_VEK_DETAIL_DEFINE_VEK_COMMON_TYPE(ui16, std::uint16_t);
SYSML_VEK_DETAIL_DEFINE_VEK_COMMON_TYPE(u32, std::uint32_t);
SYSML_VEK_DETAIL_DEFINE_VEK_COMMON_TYPE(u64, std::uint64_t);

SYSML_VEK_DETAIL_DEFINE_VEK_COMMON_TYPE(f, float);
SYSML_VEK_DETAIL_DEFINE_VEK_COMMON_TYPE(d, double);
SYSML_VEK_DETAIL_DEFINE_VEK_COMMON_TYPE(ld, long double);

#undef SYSML_VEK_DETAIL_DEFINE_VEK_COMMON_TYPE

} // namespace sysml
