// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include <catch2/catch.hpp>

#include "sysml/code_generator/meta_mnemonics.hpp"

#define META_MNEMONICS_TEST_LIVE_INCLUDE

#if defined(SYSML_CODE_GENERATOR_ARCHITECTURE_AARCH64)

#include "arm/meta_mnemonics/stack.cpp"
#include "arm/meta_mnemonics/large_imm.cpp"

#endif

#undef META_MNEMONICS_TEST_LIVE_INCLUDE
