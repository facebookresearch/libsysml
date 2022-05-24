// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include <cstddef>
#include <cstdlib>
#include <new>

namespace sysml
{

namespace detail
{

inline void* aligned_alloc_impl(std::size_t alignment, std::size_t size)
{
    // Workaround c++17 alligned_alloc (will get it from C11's stdlib.h header
    // otherwise);
    using namespace std;

    return aligned_alloc(alignment, size);
}

} // namespace detail

inline void* aligned_alloc(std::size_t alignment, std::size_t size)
{
    return detail::aligned_alloc_impl(alignment, size);
}

inline void* checked_aligned_alloc(std::size_t alignment, std::size_t size)
{
    auto ret = aligned_alloc(size, alignment);

    if (!ret)
    {
        throw std::bad_alloc();
    }

    return ret;
}

inline void aligned_free(void* ptr) noexcept { return std::free(ptr); }

} // namespace sysml
