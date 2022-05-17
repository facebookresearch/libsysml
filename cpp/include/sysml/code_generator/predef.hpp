// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) ||           \
    defined(__x86_64)
#    define SYSML_CODE_GENERATOR_ARCHITECTURE_AMD64
#elif defined(__aarch64__)
#    define SYSML_CODE_GENERATOR_ARCHITECTURE_AARCH64
#else
#    error "Target architecture not supported by sysml::code_generator library"
#endif
