// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/bits/ndarray/types.hpp"
#include "sysml/vek.hpp"

#include <array>       // for std::array
#include <type_traits> // for std::integral_constant, std::is_same_v

namespace sysml
{
namespace ndarray_detail
{

struct row_major_stride_compute
{
    template <unsigned Nd, index_array_like Tp>
    std::array<types::index_type, Nd>
    operator()(std::integral_constant<unsigned, Nd>,
               Tp const& shape) const noexcept
    {
        static_assert(Nd > 0);
        std::array<types::index_type, Nd> ret;
        ret[Nd - 1] = 1;
        for (unsigned i = Nd - 1; i > 0; --i)
        {
            ret[i - 1] = ret[i] * shape[i];
        }
        return ret;
    }
};

struct column_major_stride_compute
{
    template <unsigned Nd, index_array_like Tp>
    std::array<types::index_type, Nd>
    operator()(std::integral_constant<unsigned, Nd>,
               Tp const& shape) const noexcept
    {
        static_assert(Nd > 0);
        std::array<types::index_type, Nd> ret;
        ret[0] = 1;
        for (unsigned i = 1; i < Nd; ++i)
        {
            ret[i] = ret[i - 1] * shape[i - 1];
        }
        return ret;
    }
};

template <class Tp, unsigned Nd, index_array_like U>
constexpr std::array<Tp, Nd> to_std_array(U const& a) noexcept
{
    std::array<Tp, Nd> ret;
    for (unsigned i = 0; i < Nd; ++i)
        ret[i] = a[i];
    return ret;
}

} // namespace ndarray_detail

template <class T>
concept ndarray_order_kind =
    std::is_same_v<T, ndarray_detail::row_major_stride_compute> ||
    std::is_same_v<T, ndarray_detail::column_major_stride_compute>;

namespace
{
    [[maybe_unused]] ndarray_detail::row_major_stride_compute const
        row_major_order;
    [[maybe_unused]] ndarray_detail::column_major_stride_compute const
        column_major_order;
} // anonymous namespace

} // namespace sysml
