// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include <cassert>
#include <stdexcept>

#define SYSML_ASSERT assert

#define SYSML_STRINGIFY_0(s) #s
#define SYSML_STRINGIFY(s) SYSML_STRINGIFY_0(s)

#define SYSML_STRONG_ASSERT(condition)                                         \
    if (!(condition))                                                          \
    {                                                                          \
        throw std::runtime_error(                                              \
            SYSML_STRINGIFY(condition) " failed file: " __FILE__               \
                                       " line: " SYSML_STRINGIFY((__LINE__))); \
    }                                                                          \
    static_cast<void>(0)
