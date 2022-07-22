// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/ndarray/ref.hpp"
#include "sysml/memory.hpp"

namespace sysml
{

template <arithmetic Tp, unsigned Nd>
class ndarray : public ndarray_ref<Tp, Nd>

{
private:
    using this_type  = ndarray<Tp, Nd>;
    using super_type = ndarray_ref<Tp, Nd>;

    static const unsigned default_alignment = 64;
    static const unsigned default_padding   = 64;

public:
    using element         = typename super_type::element;
    using size_type       = typename super_type::size_type;
    using index_type      = typename super_type::index_type;
    using reference       = typename super_type::reference;
    using const_reference = typename super_type::const_reference;

private:
    static Tp* get_memory(std::array<size_type, Nd> const& dims)
    {
        auto total_elements = std::accumulate(
            dims.begin(), dims.end(), size_type(1), std::multiplies<>{});

        return static_cast<Tp*>(checked_aligned_allocate(
            default_alignment, total_elements, default_padding));
    }

public:
    template <
        ndarray_order_kind Order = ndarray_detail::row_major_stride_compute>
    ndarray(std::array<size_type, Nd> const& dims, Order o = Order{})
        : super_type(this_type::get_memory(dims), dims, o)
    {
    }

    ndarray()
        : super_type(nullptr, {}, {})
    {
    }

    ndarray(ndarray&& other)
        : super_type(std::exchange(other.ptr_, nullptr), other.native_shape(),
                     other.native_strides())
    {
    }

    ndarray& operator=(ndarray&& other)
    {
        this->ptr_     = std::exchange(other.ptr_, nullptr);
        this->profile_ = other.profile_;
        return *this;
    }

    template <class OPtr, bool OIsProxy>
    ndarray&
    operator=(ndarray_detail::const_ndarray_view_base<Tp, Nd, OIsProxy,
                                                      OPtr> const& other)
    {
        super_type::assign_from(other);
        return *this;
    }

    ndarray& operator=(ndarray const& other)
    {
        super_type::assign_from(other);
        return *this;
    }

    ~ndarray()
    {
        if (this->ptr_)
        {
            aligned_free(this->ptr_);
        }
    }

    // template <class OPtr, bool OIsProxy>
    // ndarray_ref&
    // operator=(ndarray_detail::const_ndarray_view_base<Tp, Nd, OIsProxy,
    //                                                   OPtr> const& other)
    // {
    //     super_type::assign_from(other);
    //     return *this;
    // }
};

} // namespace sysml
