// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/bits/ndarray/fwd.hpp"
#include "sysml/bits/ndarray/utility.hpp"

#include <iostream>

namespace sysml::ndarray_detail
{

template <arithmetic Tp, unsigned Nd, bool IsProxy, class TPtr = Tp const*>
class const_ndarray_view_base
{
private:
    static_assert(std::is_pointer_v<TPtr>);
    static_assert(std::is_same_v<std::remove_pointer_t<TPtr>, Tp> ||
                  std::is_same_v<std::remove_pointer_t<TPtr>, Tp const>);

private:
    using ndsubarray_accessor_type = ndsubarray_accessor<Tp, Nd>;

public:
    using element         = Tp;
    using size_type       = types::size_type;
    using index_type      = types::index_type;
    using reference       = typename ndsubarray_accessor_type::reference;
    using const_reference = typename ndsubarray_accessor_type::const_reference;

private:
    using shape_storage   = select_storage_t<types::size_type, Nd, IsProxy>;
    using strides_storage = select_storage_t<types::index_type, Nd, IsProxy>;

    using shape_list   = typename shape_storage::type;
    using strides_list = typename strides_storage::type;

protected:
    TPtr         ptr_;
    shape_list   shape_;
    strides_list strides_;

public:
    template <class OPtr>
    const_ndarray_view_base(
        const_ndarray_view_base<Tp, Nd, IsProxy, OPtr> const& other) noexcept
        : ptr_(other.data())
        , shape_(other.shape_)
        , strides_(other.strides_)
    {
        static_assert(std::is_same_v<std::remove_pointer_t<OPtr>, Tp> ||
                      std::is_same_v<std::remove_pointer_t<OPtr>, Tp const>);
    }

public:
    TPtr             data() const noexcept { return ptr_; }
    size_type        size() const noexcept { return shape_[0]; }
    bool             empty() const noexcept { return shape_[0] == 0; }
    unsigned         dimensionality() const noexcept { return Nd; }
    unsigned         num_dimensions() const noexcept { return Nd; }
    size_type const* shape() const noexcept
    {
        return shape_storage::to_pointer(shape_);
    }
    index_type const* strides() const noexcept
    {
        return strides_storage::to_pointer(strides_);
    }
    size_type num_elements() const noexcept
    {
        return std::accumulate(shape_, shape_ + dimensionality(), size_type(1),
                               std::multiplies<>{});
    }

    const_reference at(index_type idx) const
    {
        return ndsubarray_accessor_type::template access_value_at<true>(
            std::true_type{}, std::type_identity<const_reference>{}, idx, ptr_,
            shape(), strides());
    }

    const_reference operator[](index_type idx) const
    {
        return ndsubarray_accessor_type::template access_value_at<false>(
            std::false_type{}, std::type_identity<const_reference>{}, idx, ptr_,
            shape(), strides());
    }

    template <std::integral IndexT>
    Tp const& at(std::array<IndexT, Nd> const& idx) const
    {
        return ndsubarray_accessor_type::template access_element_at<true>(
            std::true_type{}, std::type_identity<Tp const&>{}, idx, ptr_,
            shape(), strides());
    }

    template <std::integral IndexT>
    Tp const& operator[](std::array<IndexT, Nd> const& idx) const
    {
        return ndsubarray_accessor_type::template access_element_at<false>(
            std::false_type{}, std::type_identity<Tp const&>{}, idx, ptr_,
            shape(), strides());
    }

    Tp const& at(std::array<int, Nd> const& idx) const
    {
        return ndsubarray_accessor_type::template access_element_at<true>(
            std::true_type{}, std::type_identity<Tp const&>{}, idx, ptr_,
            shape(), strides());
    }

    Tp const& operator[](std::array<int, Nd> const& idx) const
    {
        return ndsubarray_accessor_type::template access_element_at<false>(
            std::false_type{}, std::type_identity<Tp const&>{}, idx, ptr_,
            shape(), strides());
    }

protected:
    template <arithmetic, unsigned>
    friend struct ndsubarray_accessor;
    template <arithmetic, unsigned, bool, class>
    friend class const_ndarray_view_base;

    const_ndarray_view_base(TPtr ptr, shape_list const& shape,
                            strides_list const& strides)
        : ptr_(ptr)
        , shape_(shape)
        , strides_(strides)
    {
    }
};

} // namespace sysml::ndarray_detail
