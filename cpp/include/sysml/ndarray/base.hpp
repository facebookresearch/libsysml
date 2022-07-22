// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/ndarray/descriptor.hpp"
#include "sysml/ndarray/fwd.hpp"
#include "sysml/ndarray/utility.hpp"

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

protected:
    using ndsubarray_accessor_type = ndsubarray_accessor<Tp, Nd>;

public:
    using element         = Tp;
    using size_type       = types::size_type;
    using index_type      = types::index_type;
    using reference       = typename ndsubarray_accessor_type::reference;
    using const_reference = typename ndsubarray_accessor_type::const_reference;

protected:
    static const ownership_kind kind =
        IsProxy ? ownership_kind::nonowning : ownership_kind::owning;

    using ndarray_profile_type = ndarray_profile<Nd, kind>;

protected:
    TPtr                 ptr_;
    ndarray_profile_type profile_;

public:
    template <class OPtr>
    const_ndarray_view_base(
        const_ndarray_view_base<Tp, Nd, IsProxy, OPtr> const& other) noexcept
        : ptr_(other.data())
        , profile_(other.profile_)
    {
        static_assert(std::is_same_v<std::remove_pointer_t<OPtr>, Tp> ||
                      std::is_same_v<std::remove_pointer_t<OPtr>, Tp const>);
    }

private:
    const_ndarray_view_base& operator=(const_ndarray_view_base const&) = delete;

public:
    TPtr      data() const noexcept { return ptr_; }
    size_type size() const noexcept { return shape()[0]; }
    bool      empty() const noexcept { return shape()[0] == 0; }
    unsigned  dimensionality() const noexcept { return Nd; }
    unsigned  num_dimensions() const noexcept { return Nd; }

    decltype(auto) native_shape() const noexcept
    {
        return profile_.native_extents_data();
    }
    decltype(auto) native_strides() const noexcept
    {
        return profile_.native_strides_data();
    }

    size_type const*  shape() const noexcept { return profile_.extents_data(); }
    index_type const* strides() const noexcept
    {
        return profile_.strides_data();
    }
    size_type num_elements() const noexcept
    {
        return std::accumulate(shape(), shape() + dimensionality(),
                               size_type(1), std::multiplies<>{});
    }

    const_reference at(index_type idx) const
    {
        return ndsubarray_accessor_type::access_value_at(
            std::true_type{}, std::type_identity<const_reference>{}, idx, ptr_,
            shape(), strides());
    }

    const_reference operator[](index_type idx) const
    {
        return ndsubarray_accessor_type::access_value_at(
            std::false_type{}, std::type_identity<const_reference>{}, idx, ptr_,
            shape(), strides());
    }

    template <std::integral IndexT>
    Tp const& at(std::array<IndexT, Nd> const& idx) const
    {
        return ndsubarray_accessor_type::access_element_at(
            std::true_type{}, std::type_identity<Tp const&>{}, idx, ptr_,
            shape(), strides());
    }

    template <std::integral IndexT>
    Tp const& operator[](std::array<IndexT, Nd> const& idx) const
    {
        return ndsubarray_accessor_type::access_element_at(
            std::false_type{}, std::type_identity<Tp const&>{}, idx, ptr_,
            shape(), strides());
    }

    Tp const& at(std::array<int, Nd> const& idx) const
    {
        return ndsubarray_accessor_type::access_element_at(
            std::true_type{}, std::type_identity<Tp const&>{}, idx, ptr_,
            shape(), strides());
    }

    Tp const& operator[](std::array<int, Nd> const& idx) const
    {
        return ndsubarray_accessor_type::access_element_at(
            std::false_type{}, std::type_identity<Tp const&>{}, idx, ptr_,
            shape(), strides());
    }

protected:
    template <arithmetic, unsigned>
    friend struct ndsubarray_accessor;
    template <arithmetic, unsigned, bool, class>
    friend class const_ndarray_view_base;

    const_ndarray_view_base(
        TPtr ptr, typename ndarray_profile_type::native_extents_type const& e,
        typename ndarray_profile_type::native_strides_type const& s)
        : ptr_(ptr)
        , profile_{e, s}
    {
    }
};

template <arithmetic Tp, unsigned Nd, bool IsProxy>
class ndarray_view_base : public const_ndarray_view_base<Tp, Nd, IsProxy, Tp*>
{
private:
    using super_type = const_ndarray_view_base<Tp, Nd, IsProxy, Tp*>;

protected:
    using super_type::super_type;

    using ndarray_profile_type = typename super_type::ndarray_profile_type;
    using ndsubarray_accessor_type =
        typename super_type::ndsubarray_accessor_type;

    template <class OPtr, bool OIsProxy>
    void
    assign_from(const_ndarray_view_base<Tp, Nd, OIsProxy, OPtr> const& other)
    {
        static_assert(std::is_same_v<std::remove_pointer_t<OPtr>, Tp> ||
                      std::is_same_v<std::remove_pointer_t<OPtr>, Tp const>);

        assert(super_type::shape()[0] == other.shape()[0]);

        for (size_type i = 0; i < super_type::shape()[0]; ++i)
        {
            operator[](i) = other[i];
        }
    }

public:
    using element         = typename super_type::element;
    using size_type       = typename super_type::size_type;
    using index_type      = typename super_type::index_type;
    using reference       = typename super_type::reference;
    using const_reference = typename super_type::const_reference;

public:
    using super_type::at;
    using super_type::operator[];

    reference at(index_type idx)
    {
        return ndsubarray_accessor_type::access_value_at(
            std::true_type{}, std::type_identity<reference>{}, idx,
            super_type::ptr_, super_type::shape(), super_type::strides());
    }

    reference operator[](index_type idx)
    {
        return ndsubarray_accessor_type::access_value_at(
            std::false_type{}, std::type_identity<reference>{}, idx,
            super_type::ptr_, super_type::shape(), super_type::strides());
    }

    template <std::integral IndexT>
    Tp& at(std::array<IndexT, Nd> const& idx)
    {
        return ndsubarray_accessor_type::access_element_at(
            std::true_type{}, std::type_identity<Tp&>{}, idx, super_type::ptr_,
            super_type::shape(), super_type::strides());
    }

    template <std::integral IndexT>
    Tp& operator[](std::array<IndexT, Nd> const& idx)
    {
        return ndsubarray_accessor_type::access_element_at(
            std::false_type{}, std::type_identity<Tp&>{}, idx, super_type::ptr_,
            super_type::shape(), super_type::strides());
    }

    Tp& at(std::array<int, Nd> const& idx)
    {
        return ndsubarray_accessor_type::access_element_at(
            std::true_type{}, std::type_identity<Tp&>{}, idx, super_type::ptr_,
            super_type::shape(), super_type::strides());
    }

    Tp& operator[](std::array<int, Nd> const& idx)
    {
        return ndsubarray_accessor_type::access_element_at(
            std::false_type{}, std::type_identity<Tp&>{}, idx, super_type::ptr_,
            super_type::shape(), super_type::strides());
    }

    template <class OPtr, bool OIsProxy>
    ndarray_view_base&
    operator=(const_ndarray_view_base<Tp, Nd, OIsProxy, OPtr> const& other)
    {
        assign_from(other);
        return *this;
    }
};

} // namespace sysml::ndarray_detail
