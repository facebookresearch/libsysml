// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/ndarray/types.hpp"

#include <array>
#include <cassert>
#include <type_traits>

namespace sysml::ndarray_detail
{

enum struct ownership_kind
{
    owning,
    nonowning
};

template <unsigned Nd, ownership_kind OwningKind>
struct ndarray_profile
{
    static_assert(Nd > 0);

    static const bool IsOwning = OwningKind == ownership_kind::owning;

    using extent_type = types::size_type;
    using stride_type = types::size_type;

    using native_extents_type =
        std::conditional_t<IsOwning, std::array<extent_type, Nd>,
                           extent_type const*>;
    using native_strides_type =
        std::conditional_t<IsOwning, std::array<stride_type, Nd>,
                           stride_type const*>;

    native_extents_type extents;
    native_strides_type strides;

    constexpr extent_type const* extents_data() const noexcept
    {
        if constexpr (IsOwning)
        {
            return extents.data();
        }
        else
        {
            return extents;
        }
    }
    constexpr stride_type const* strides_data() const noexcept
    {
        if constexpr (IsOwning)
        {
            return strides.data();
        }
        else
        {
            return strides;
        }
    }

    constexpr native_extents_type const& native_extents_data() const noexcept
    {
        return extents;
    }
    constexpr native_strides_type const& native_strides_data() const noexcept
    {
        return strides;
    }

    constexpr extent_type const* subextents_data() const noexcept
    {
        return extents_data() + 1;
    }
    constexpr stride_type const* substrides_data() const noexcept
    {
        return strides_data() + 1;
    }
};

template <unsigned Nd>
using owning_ndarray_profile = ndarray_profile<Nd, ownership_kind::owning>;
template <unsigned Nd>
using nonowning_ndarray_profile = ndarray_profile<Nd, ownership_kind::nonowning>;

namespace detail
{
template <ownership_kind WasOwningKind, std::size_t... Idx>
constexpr ndarray_profile<sizeof...(Idx), ownership_kind::owning>
get_owning_subprofile(
    std::index_sequence<Idx...>,
    ndarray_profile<sizeof...(Idx) + 1, WasOwningKind> const& in) noexcept
{
    return {{in.extents_data()[Idx + 1]...}, {in.strides_data()[Idx + 1]...}};
}
} // namespace detail

template <ownership_kind OwningKind, unsigned Nd, ownership_kind WasOwningKind>
constexpr ndarray_profile<Nd - 1, OwningKind>
get_subprofile(ndarray_profile<Nd, WasOwningKind> const& in) noexcept
{
    static_assert(Nd > 1);

    if constexpr (OwningKind == ownership_kind::owning)
    {
        return detail::get_owning_subprofile(std::make_index_sequence<Nd - 1>{},
                                             in);
    }
    else
    {
        return {in.extents_data() + 1, in.strides_data() + 1};
    }
}

} // namespace sysml::ndarray_detail
