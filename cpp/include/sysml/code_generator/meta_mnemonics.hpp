// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/code_generator/predef.hpp"
#include "sysml/predef.hpp"

#if defined(SYSML_CODE_GENERATOR_ARCHITECTURE_AMD64)

#elif defined(SYSML_CODE_GENERATOR_ARCHITECTURE_AARCH64)

#include "sysml/code_generator/meta_mnemonics/arm/stack.hpp"
#include "sysml/code_generator/meta_mnemonics/arm/large_imm.hpp"
#include "sysml/code_generator/meta_mnemonics/arm/vreg_transpose.hpp"
#include "sysml/code_generator/meta_mnemonics/arm/m1_accelerate.hpp"

#else
#    error "Target architecture not supported by sysml::code_generator library"
#endif
