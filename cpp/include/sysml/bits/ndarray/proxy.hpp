// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/bits/ndarray/algorithm.hpp"
#include "sysml/bits/ndarray/base.hpp"

#include <concepts>

namespace sysml
{

template <arithmetic Tp, unsigned Nd, class TPtr>
class const_ndarray_view_proxy
    : public ndarray_detail::const_ndarray_view_base<Tp, Nd, true, TPtr>
{
private:
    using super_type =
        ndarray_detail::const_ndarray_view_base<Tp, Nd, true, TPtr>;

public:
    using element         = typename super_type::element;
    using size_type       = typename super_type::size_type;
    using index_type      = typename super_type::index_type;
    using reference       = typename super_type::reference;
    using const_reference = typename super_type::const_reference;

    using super_type::super_type;

    // template <
    //     ndarray_order_kind Order = ndarray_detail::row_major_stride_compute>
    // const_ndarray_view_proxy(TPtr ptr, size_type const* shape,
    //                          Order o = Order{}) noexcept
    //     : super_type(ptr, shape,
    //                  o(std::integral_constant<unsigned, Nd>{}, shape).data())
    // {
    // }
};

} // namespace sysml
