// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/ndarray/algorithm.hpp"
#include "sysml/ndarray/base.hpp"

#include <concepts>
#include <iostream>

namespace sysml
{

template <arithmetic Tp, unsigned Nd, class TPtr>
class const_ndarray_view
    : public ndarray_detail::const_ndarray_view_base<Tp, Nd, false, TPtr>
{
private:
    using super_type =
        ndarray_detail::const_ndarray_view_base<Tp, Nd, false, TPtr>;

public:
    using element         = typename super_type::element;
    using size_type       = typename super_type::size_type;
    using index_type      = typename super_type::index_type;
    using reference       = typename super_type::reference;
    using const_reference = typename super_type::const_reference;

    using super_type::super_type;

    template <
        ndarray_order_kind Order = ndarray_detail::row_major_stride_compute>
    const_ndarray_view(TPtr ptr, size_type const* shape,
                       Order o = Order{}) noexcept
        : super_type(ptr, shape,
                     o(std::integral_constant<unsigned, Nd>{}, shape).data())
    {
        std::cout << "Created strides: " << super_type::strides()[0] << ", "
                  << super_type::strides()[1] << "\n\n";
    }

    template <arithmetic IndexT, ndarray_order_kind Order =
                                     ndarray_detail::row_major_stride_compute>
    const_ndarray_view(TPtr ptr, std::array<IndexT, Nd> const& shape,
                       Order o = Order{}) noexcept
        : super_type(
              ptr, shape.data(),
              o(std::integral_constant<unsigned, Nd>{}, shape.data()).data())
    {
        std::cout << "Created strides: " << super_type::strides()[0] << ", "
                  << super_type::strides()[1] << "\n\n";
    }
};

} // namespace sysml
