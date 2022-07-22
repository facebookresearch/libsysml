// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/ndarray/algorithm.hpp"
#include "sysml/ndarray/base.hpp"

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

template <arithmetic Tp, unsigned Nd>
class ndarray_view_proxy
    : public ndarray_detail::ndarray_view_base<Tp, Nd, true>
{
private:
    using super_type = ndarray_detail::ndarray_view_base<Tp, Nd, true>;

public:
    using element         = typename super_type::element;
    using size_type       = typename super_type::size_type;
    using index_type      = typename super_type::index_type;
    using reference       = typename super_type::reference;
    using const_reference = typename super_type::const_reference;

    using super_type::super_type;

    using super_type::operator[];

    // template <class OPtr, bool IsProxy>
    // ndarray_view_proxy&
    // operator=(ndarray_detail::const_ndarray_view_base<Tp, Nd, IsProxy,
    //                                                   OPtr> const& other)
    // {
    //     static_assert(std::is_same_v<std::remove_pointer_t<OPtr>, Tp> ||
    //                   std::is_same_v<std::remove_pointer_t<OPtr>, Tp const>);
    //     std::cout << "HEREREREE -------------- ZIZIZI\n";
    //     super_type::operator=(other);
    //     return *this;
    // }

    template <class OPtr, bool OIsProxy>
    ndarray_view_proxy&
    operator=(ndarray_detail::const_ndarray_view_base<Tp, Nd, OIsProxy,
                                                      OPtr> const& other)
    {
        super_type::assign_from(other);
        return *this;
    }

    ndarray_view_proxy& operator=(ndarray_view_proxy const& other)
    {
        super_type::assign_from(other);
        return *this;
    }
};

} // namespace sysml
