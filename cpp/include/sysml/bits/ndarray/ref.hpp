// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/bits/ndarray/algorithm.hpp"
#include "sysml/bits/ndarray/base.hpp"

#include <concepts>
#include <iostream>

namespace sysml
{

template <arithmetic Tp, unsigned Nd>
class const_ndarray_ref
    : public ndarray_detail::const_ndarray_view_base<Tp, Nd, false, Tp const*>

{
private:
    using super_type =
        ndarray_detail::const_ndarray_view_base<Tp, Nd, false, Tp const*>;

public:
    using element         = typename super_type::element;
    using size_type       = typename super_type::size_type;
    using index_type      = typename super_type::index_type;
    using reference       = typename super_type::reference;
    using const_reference = typename super_type::const_reference;

    // const_ndarray_ref() noexcept
    //     : super_type(nullptr, {}, {})
    // {
    // }

    template <
        // ndarray_detail::index_array_like IndexArrayType,
        // arithmetic T,
        ndarray_order_kind Order = ndarray_detail::row_major_stride_compute>
    const_ndarray_ref(Tp const* ptr, std::array<size_type, Nd> const& shape, //IndexArrayType const& shape,
                      Order o = Order{}) noexcept
        : super_type(ptr, shape,
                     o(std::integral_constant<unsigned, Nd>{}, shape))
    {
        std::cout << "ASDASDASDAS: " << std::is_integral_v<int const> << "\n\n";
    }

    // template <arithmetic IndexT, ndarray_order_kind Order =
    //           ndarray_detail::row_major_stride_compute>
    // const_ndarray_ref(Tp const* ptr, std::array<IndexT, Nd> const& shape,
    //                   Order o = Order{}) noexcept
    //     : super_type(
    //           ptr, shape.data(),
    //           o(std::integral_constant<unsigned, Nd>{}, shape.data()).data())
    // {
    //
    // }
};

} // namespace sysml
