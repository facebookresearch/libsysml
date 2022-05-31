// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#if defined(__aarch64__) && defined(__APPLE__) && defined(__clang__)
#    define SYSML_HAS___Float16 1
#endif

#if defined(__aarch64__) && defined(__APPLE__) && defined(__GNUC__)
#    define SYSML_HAS___fp16 1
#endif
