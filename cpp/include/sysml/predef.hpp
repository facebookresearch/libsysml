// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

// Inspired by boost predef

#define SYSML_VERSION_NUMBER(major, minor, patch)                              \
    ((((major) % 100) * 10000000) + (((minor) % 100) * 100000) +               \
     ((patch) % 100000))

#if defined(__clang__)

#    define SYSML_COMP_CLANG                                                   \
        SYSML_VERSION_NUMBER(__clang_major__, __clang_minor__,                 \
                             __clang_patchlevel__)

#elif defined(__GNUC__)

#    define SYSML_COMP_GNUC                                                    \
        SYSML_VERSION_NUMBER(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)

#else

#    error "Compiler not supported"

#endif

// Detect architecture (but not the extension)
// Checkout: https://sourceforge.net/p/predef/wiki/Architectures/

#if defined(SYSML_ARCH)
#    error "SYSML_ARCH can't be defined at this junction"
#endif

#define SYSML_ARCH_UNKNOWN 0x00000000
#define SYSML_ARCH_I386 0x00010000
#define SYSML_ARCH_AMD64 0x00010001
#define SYSML_ARCH_ARM64 0x00020000

#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) ||           \
    defined(__x86_64) /* defined by GCC and Sun Studio (and clang?) */ ||      \
    defined(_M_X64) || defined(_M_AMD64) /* defined by Visual Studio */
#    define SYSML_ON_ARCH_AMD64
#    define SYSML_ARCH SYSML_ARCH_AMD64
#elif defined(__aarch64__) /* defined by GNU C 1 */
#    define SYSML_ON_ARCH_ARM64
#    define SYSML_ARCH SYSML_ARCH_ARM64
#endif

#if !defined(SYSML_ARCH) /* AMD64 is also I386, but not vice versa */
#    if defined(i386) || defined(__i386) || defined(__i386__) /* GNU C */ ||   \
        defined(__i386) /* Sun Studio */ ||                                    \
        defined(_M_IX86) /* Visual Studio, Intel C/C++, Digital Mars, and      \
                            Watcom C/C++ */                                    \
        || defined(__X86__) /* Watcom C/C++ */ ||                              \
        defined(_X86_) /* MinGW32 */ ||                                        \
        defined(__THW_INTEL__) /* XL C/C++ */ ||                               \
        defined(__I86__) /* Digital Mars */ ||                                 \
        defined(__INTEL__) /* __X86__ */ || defined(__386) /* Diab */
#        define SYSML_ON_ARCH_I386
#        define SYSML_ARCH SYSML_ARCH_I386
#    endif
#endif

#if !defined(SYSML_ARCH)
#    define SYSML_ON_ARCH_UNKNOWN
#    define SYSML_ARCH SYSML_ARCH_UNKNOWN
#endif
