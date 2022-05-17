// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include <cstddef> // for std::size_t
#include <concepts> // for std::convertible_to

namespace sysml::ndarray_detail
{

struct types
{
    using size_type  = std::size_t;
    using index_type = std::size_t;
};

template <typename T>
concept index_array_like = requires(T a)
{
    // clang-format off
    { a[0] } -> std::convertible_to<types::index_type>;
    // clang-format on
};

} // namespace sysml::ndarray_detail
