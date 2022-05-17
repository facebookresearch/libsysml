// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/code_generator/predef.hpp"
#include "sysml/predef.hpp"

#if defined(SYSML_CODE_GENERATOR_ARCHITECTURE_AMD64)

#    if SYSML_COMP_GNUC
#        if SYSML_COMP_GNUC >= SYSML_VERSION_NUMBER(11, 0, 0)
#            define SYSML_XBYAK_SUPRESS_WARRAY_BOUNDS
#        endif
#    endif

#    if !defined(XBYAK_NO_OP_NAMES)
#        define XBYAK_NO_OP_NAMES
#    endif

#    ifdef SYSML_XBYAK_SUPRESS_WARRAY_BOUNDS
#        pragma GCC diagnostic push
#        pragma GCC diagnostic ignored "-Warray-bounds"
#    endif

#    include <xbyak/xbyak.h>
#    include <xbyak/xbyak_util.h>

#    ifdef SYSML_XBYAK_SUPRESS_WARRAY_BOUNDS
#        pragma GCC diagnostic pop
#        undef SYSML_XBYAK_SUPRESS_WARRAY_BOUNDS
#    endif

namespace sysml::code_generator
{
namespace amd64::xbyak
{

using namespace Xbyak;

using buffer_type = Xbyak::uint8;

using code_array     = CodeArray;
using allocator      = Allocator;
using code_generator = CodeGenerator;
using reg64          = Reg64;
using label          = Label;

} // namespace amd64::xbyak

namespace xbyak = amd64::xbyak;

} // namespace sysml::code_generator

#elif defined(SYSML_CODE_GENERATOR_ARCHITECTURE_AARCH64)

#    include <cstdint>
#    include <xbyak_aarch64/xbyak_aarch64.h>

namespace sysml::code_generator
{
namespace aarch64::xbyak
{

using namespace Xbyak_aarch64;

using CodeArray     = CodeArrayAArch64;
using Allocator     = AllocatorAArch64;
using CodeGenerator = CodeGeneratorAArch64;
using Reg64         = XReg;
using Label         = LabelAArch64;

using buffer_type = std::uint32_t;

using code_array     = CodeArray;
using allocator      = Allocator;
using code_generator = CodeGenerator;
using reg64          = Reg64;
using label          = Label;

} // namespace aarch64::xbyak

namespace xbyak = aarch64::xbyak;

} // namespace sysml::code_generator

#else
#    error "Target architecture not supported by sysml::code_generator library"
#endif
