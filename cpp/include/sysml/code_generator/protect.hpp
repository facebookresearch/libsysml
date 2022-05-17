// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include <cstddef>

namespace sysml::code_generator
{

enum struct memory_protection_mode
{
    rw,
    rwe,
    re
};

inline bool protect(void const* addr, std::size_t size,
                    memory_protection_mode protect_mode)
{
    int mode = 0;

    switch (protect_mode)
    {
    case memory_protection_mode::rw:
        mode = PROT_READ | PROT_WRITE;
        break;
    case memory_protection_mode::rwe:
        mode = PROT_READ | PROT_WRITE | PROT_EXEC;
        break;
    case memory_protection_mode::re:
        mode = PROT_READ | PROT_EXEC;
        break;
    default:
        return false;
    }

#if defined(__GNUC__)
    std::size_t page_size = ::sysconf(_SC_PAGESIZE);
    std::size_t iaddr     = reinterpret_cast<std::size_t>(addr);
    std::size_t rounded_addr =
        iaddr & ~(page_size - static_cast<std::size_t>(1));

    return ::mprotect(reinterpret_cast<void*>(rounded_addr),
                      size + (iaddr - rounded_addr), mode) == 0;
#else
    return true;
#endif
}

} // namespace sysml::code_generator
