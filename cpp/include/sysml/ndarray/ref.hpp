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

    template <
        ndarray_order_kind Order = ndarray_detail::row_major_stride_compute>
    const_ndarray_ref(
        Tp const*                        ptr,
        std::array<size_type, Nd> const& shape, // IndexArrayType const& shape,
        Order                            o = Order{}) noexcept
        : super_type(ptr, shape,
                     o(std::integral_constant<unsigned, Nd>{}, shape))
    {
    }
};

template <arithmetic Tp, unsigned Nd>
class ndarray_ref : public ndarray_detail::ndarray_view_base<Tp, Nd, false>

{
private:
    using super_type = ndarray_detail::ndarray_view_base<Tp, Nd, false>;

public:
    using element         = typename super_type::element;
    using size_type       = typename super_type::size_type;
    using index_type      = typename super_type::index_type;
    using reference       = typename super_type::reference;
    using const_reference = typename super_type::const_reference;

    template <
        ndarray_order_kind Order = ndarray_detail::row_major_stride_compute>
    ndarray_ref(
        Tp*                              ptr,
        std::array<size_type, Nd> const& shape, // IndexArrayType const& shape,
        Order                            o = Order{}) noexcept
        : super_type(ptr, shape,
                     o(std::integral_constant<unsigned, Nd>{}, shape))
    {
    }

    ndarray_ref(Tp* ptr, std::array<size_type, Nd> const& shape,
                std::array<size_type, Nd> const& strides) noexcept
        : super_type(ptr, shape, strides)
    {
    }

    template <class OPtr, bool OIsProxy>
    ndarray_ref&
    operator=(ndarray_detail::const_ndarray_view_base<Tp, Nd, OIsProxy,
                                                      OPtr> const& other)
    {
        super_type::assign_from(other);
        return *this;
    }

    ndarray_ref& operator=(ndarray_ref const& other)
    {
        super_type::assign_from(other);
        return *this;
    }
};

} // namespace sysml
