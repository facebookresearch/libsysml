// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/tensor/base.hpp"
#include "sysml/tensor/fwd.hpp"
#include "sysml/vek.hpp"

namespace sysml::ndarray_detail
{

struct sizes_as_pointers
{
    using extents_holder_t = std::size_t const*;
    using strides_holder_t = std::size_t const*;

    static std::size_t const* extents_data(extents_holder_t d) noexcept
    {
        return d;
    }

    static std::size_t const* strides_data(strides_holder_t d) noexcept
    {
        return d;
    }

    static extents_holder_t extents_from_data(std::size_t const* d) noexcept
    {
        return d;
    }

    static strides_holder_t strides_from_data(std::size_t const* d) noexcept
    {
        return d;
    }
};

template <std::size_t N>
struct sizes_as_veks
{
    using extents_holder_t = vek<std::size_t, N>;
    using strides_holder_t = vek<std::size_t, N + 1>;

    static std::size_t const* extents_data(extents_holder_t d) noexcept
    {
        return d.data();
    }

    static std::size_t const* strides_data(strides_holder_t d) noexcept
    {
        return d.data();
    }

    static extents_holder_t extents_from_data(std::size_t const* d) noexcept
    {
        return vek_from_raw<N>(d);
    }

    static strides_holder_t strides_from_data(std::size_t const* d) noexcept
    {
        return vek_from_raw<N + 1>(d);
    }
};

template <arithmetic T, std::size_t N, class SizeStorage, class TPtr = T const*>
class const_ndarray_view : public ndarray_base<T, N>
{
private:
    using super_type       = ndarray_base<T, N>;
    using extents_holder_t = typename SizeStorage::extents_holder_t;
    using strides_holder_t = typename SizeStorage::strides_holder_t;

public:
    using element         = typename super_type::element;
    using const_reference = typename super_type::const_reference;

protected:
    TPtr ptr_;
    extents_holder_t extents_;
    strides_holder_t strides_;

public:
    TPtr data() const noexcept
    {
        return ptr_;
    }

    std::size_t const* strides() const noexcept
    {
        return SizeStorage::strides_data(strides_);
    }

    std::size_t const* extents() const noexcept
    {
        return SizeStorage::extents_data(extents_);
    }


};

} // namespace sysml::ndarray_detail
