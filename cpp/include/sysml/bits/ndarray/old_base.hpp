// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/bits/tensor/fwd.hpp"

namespace sysml::ndarray_detail
{

template <arithmetic T, std::size_t ND>
requires(ND > 1) class ndarray_base<T, ND>
{
public:
    using element         = T;
    using reference       = ndsubarray<T, ND - 1>;
    using const_reference = const_ndsubarray<T, ND - 1>;
    using size_type       = std::size_t;

protected:
    template <class Reference>
    requires(is_any_of_v<Reference, reference, const_reference>) Reference
        access_element(std::type_identity<Reference>, size_type /* index */,
                       T* /* data */, size_type const* /* extents */,
                       size_type const* /* strides */)
    const noexcept { return Reference{}; }
};

template <arithmetic T>
class ndarray_base<T, 1>
{
public:
    using element         = T;
    using reference       = T&;
    using const_reference = T const&;
    using size_type       = std::size_t;

protected:
    template <class Reference>
    requires(is_any_of_v<Reference, reference, const_reference>) Reference
        access_element(std::type_identity<Reference>, size_type /* index */,
                       T* /* data */, size_type const* /* extents */,
                       size_type const* /* strides */)
    const noexcept { return Reference{}; }
};


} // namespace sysml
