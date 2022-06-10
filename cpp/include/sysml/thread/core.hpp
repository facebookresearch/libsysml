// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include <cstddef>

#if defined(__aarch64__)
#    define DABUN_THREAD_CPU_RELAX() asm __volatile__("yield" ::: "memory")
#elif defined(__amd64__) || defined(__amd64) || defined(__x86_64__) ||         \
    defined(__x86_64)
#    define DABUN_THREAD_CPU_RELAX() asm __volatile__("pause" ::: "memory")
#endif

namespace sysml::thread
{

#if 0
    // ifdef __cpp_lib_hardware_interference_size
using std::hardware_constructive_interference_size;
using std::hardware_destructive_interference_size;
#else
constexpr std::size_t hardware_constructive_interference_size = 64;
constexpr std::size_t hardware_destructive_interference_size  = 64;
#endif

template <class T>
struct hardware_constructive_interference_padding
{
    char padding[hardware_constructive_interference_size -
                 sizeof(T)]; // = {'\0'};
};

} // namespace sysml::thread
