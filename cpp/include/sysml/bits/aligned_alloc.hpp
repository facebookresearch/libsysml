// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/math.hpp"

#include <cstddef>
#include <cstdlib>
#include <new>

namespace sysml
{

namespace detail
{

inline void* aligned_allocate_impl(std::size_t alignment, std::size_t size,
                                   std::size_t padding = 0)
{
    // Workaround c++17 alligned_alloc (will get it from C11's stdlib.h header
    // otherwise);
    using namespace std;

    if (size == 0)
    {
        return nullptr;
    }

    return aligned_alloc(alignment, round_up(size + padding, alignment));
}

} // namespace detail

inline void* aligned_allocate(std::size_t alignment, std::size_t size,
                              std::size_t padding = 0)
{
    return detail::aligned_allocate_impl(alignment, size, padding);
}

inline void* checked_aligned_allocate(std::size_t alignment, std::size_t size,
                                      std::size_t padding = 0)
{
    auto ret = aligned_allocate(alignment, size, padding);

    if (!ret)
    {
        throw std::bad_alloc();
    }

    return ret;
}

inline void aligned_free(void* ptr) noexcept { return std::free(ptr); }

} // namespace sysml
