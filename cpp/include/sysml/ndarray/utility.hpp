// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/assert.hpp"
#include "sysml/ndarray/fwd.hpp"
#include "sysml/ndarray/types.hpp"
#include "sysml/type_traits.hpp"

#include <algorithm>   // for std::copy_n
#include <array>       // for std::array
#include <concepts>    // for std::integral
#include <stdexcept>   // for std::out_of_range
#include <type_traits> // for std::conditional_t, std::type_identity

namespace sysml::ndarray_detail
{

// Accessor helper struct, to provide a generic way of accessing a
// subarray as well as an element of an ndarray using the operator []
// for ndarrays of any dimensions.
template <arithmetic Tp, unsigned Nd>
struct ndsubarray_accessor
{
    static_assert(Nd > 0);

    using element    = Tp;
    using size_type  = types::size_type;
    using index_type = types::index_type;
    using reference =
        std::conditional_t<Nd == 1, Tp&, ndarray_view_proxy<Tp, Nd - 1>>;
    using const_reference =
        std::conditional_t<Nd == 1, Tp const&,
                           const_ndarray_view_proxy<Tp, Nd - 1>>;

    template <class Reference, bool>
    static Reference access_dummy_at(std::type_identity<Reference>)
    {
        return reference{};
    }

    template <bool CheckThrow, class Reference, class TPtr>
    // requires(is_any_of_v<Reference, reference, const_reference>)
    static Reference access_value_at(std::integral_constant<bool, CheckThrow>,
                                     std::type_identity<Reference>,
                                     index_type idx, TPtr base,
                                     [[maybe_unused]] size_type const* shape,
                                     index_type const* strides) noexcept
    {
        assert(idx < shape[0]);

        if constexpr (CheckThrow)
        {
            // TODO(zi) Add the message
            SYSML_THROW_ASSERT_THIS_EXCEPTION(idx < shape[0],
                                              std::out_of_range);
        }

        if constexpr (Nd == 1)
        {
            return base[idx * strides[0]];
        }
        else
        {
            return Reference(base + idx * strides[0], shape + 1, strides + 1);
        }
    }

    template <bool CheckThrow, class Reference, std::integral IndexT,
              class TPtr>
    // requires(is_any_of_v<Reference, reference, const_reference>)
    static Reference access_element_at(std::integral_constant<bool, CheckThrow>,
                                       std::type_identity<Reference>,
                                       std::array<IndexT, Nd> const& location,
                                       TPtr                          base,
                                       [[maybe_unused]] size_type const* shape,
                                       index_type const* strides) noexcept
    {
        // TODO(zi) Consider folding expression (using helpers)
        for (unsigned i = 0; i < Nd; ++i)
        {
            base += location[i] * strides[i];
            assert(location[i] < shape[i]);
            if constexpr (CheckThrow)
            {
                // TODO(zi) Add the message
                SYSML_THROW_ASSERT_THIS_EXCEPTION(location[i] < shape[i],
                                                  std::out_of_range)
                    << "shape[" << i << "] = " << shape[i] << "; location[" << i
                    << "] = " << location[i];
            }
        }

        return *base;
    }
};

// Helper struct to provide a generic way of implementing the
// const_ndarray_base/ndarray_base class that can store
// shape/strides as either non-owning pointers or owning std::arary
// which will allow us to share functionality between ndarray_view and
// ndarray_view_proxy
template <typename Tp>
struct storage_const_pointer
{
    using type = Tp const*;
    static Tp const* to_pointer(type d) noexcept { return d; }
    static type      from_pointer(Tp const* d) noexcept { return d; }
};

template <typename Tp, unsigned Nd>
struct storage_std_array
{
    using type = std::array<Tp, Nd>;
    static Tp const* to_pointer(type const& a) noexcept { return a.data(); }
    // template <class U>
    // static type from_pointer(U const* d) noexcept
    // {
    //     // TODO(zi) Consider returning {{d[]...}} using folding expression
    //     // helper
    //     std::array<Tp, Nd> ret;
    //     std::copy_n(d, Nd, ret.data());
    //     return ret;
    // }
    template <index_array_like IndexArrayType>
    static type from_array(IndexArrayType const& d)
    {
        // TODO(zi) Consider returning {{d[]...}} using folding expression
        // helper
        std::array<Tp, Nd> ret;
        for (unsigned i = 0; i < Nd; ++i)
        {
            ret[i] = d[i];
        }
        return ret;
    }
};

template <typename Tp, unsigned Nd, bool IsProxy>
struct select_storage
{
    using type = std::conditional_t<IsProxy, storage_const_pointer<Tp>,
                                    storage_std_array<Tp, Nd>>;
};

template <typename Tp, unsigned Nd, bool IsProxy>
using select_storage_t = typename select_storage<Tp, Nd, IsProxy>::type;

} // namespace sysml::ndarray_detail
