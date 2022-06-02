// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/vek.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <type_traits>

namespace sysml
{
namespace ndloop_detail
{

template <std::size_t, class T>
struct just_second
{
    using type = T;
};

template <std::size_t I, class T>
using just_second_t = typename just_second<I, T>::type;

template <class Fn, class Vek>
struct is_exploded_invokable : std::false_type
{
};

template <class Fn, arithmetic T, class>
struct is_exploded_invokable_helper : std::false_type
{
};

template <class Fn, arithmetic T, std::size_t... Idx>
struct is_exploded_invokable_helper<Fn, T, std::index_sequence<Idx...>>
    : std::is_invocable<Fn const&, just_second_t<Idx, T>...>
{
};

template <class Fn, arithmetic Int, std::size_t Nm>
struct is_exploded_invokable<Fn, vek<Int, Nm>>
    : is_exploded_invokable_helper<Fn, Int, std::make_index_sequence<Nm>>
{
};

template <class Fn, class Vek>
constexpr bool is_exploded_invokable_v = is_exploded_invokable<Fn, Vek>::value;

template <class Fn, class Int, std::size_t... Idx>
__attribute__((always_inline)) inline void
invoke_exploded(Fn const& f, vek<Int, sizeof...(Idx)> const& v,
                std::index_sequence<Idx...>)
{
    f(v[Idx]...);
}

template <std::size_t Dim, bool VekCall, class Fn, class Int, std::size_t N>
__attribute__((always_inline)) inline void
for_all_helper(vek<Int, N>& var, vek<Int, N> const& begin,
               vek<Int, N> const& end, Fn const& fn)
{
    if constexpr (Dim + 1 == N)
    {
        for (var[Dim] = begin[Dim]; var[Dim] < end[Dim]; ++var[Dim])
        {
            vek<Int, N> const& cvar = var;
            if constexpr (VekCall)
            {
                fn(cvar);
            }
            else
            {
                invoke_exploded(fn, cvar, std::make_index_sequence<N>{});
            }
        }
    }
    else
    {
        for (var[Dim] = begin[Dim]; var[Dim] < end[Dim]; ++var[Dim])
        {
            for_all_helper<Dim + 1, VekCall>(var, begin, end, fn);
        }
    }
}

template <std::size_t Dim, bool VekCall, class Fn, class Int, std::size_t N>
__attribute__((always_inline)) inline void
for_all_helper(vek<Int, N>& var, vek<Int, N> const& end, Fn const& fn)
{
    if constexpr (Dim + 1 == N)
    {
        for (var[Dim] = static_cast<Int>(0); var[Dim] < end[Dim]; ++var[Dim])
        {
            vek<Int, N> const& cvar = var;
            if constexpr (VekCall)
            {
                fn(cvar);
            }
            else
            {
                invoke_exploded(fn, cvar, std::make_index_sequence<N>{});
            }
        }
    }
    else
    {
        for (var[Dim] = static_cast<Int>(0); var[Dim] < end[Dim]; ++var[Dim])
        {
            for_all_helper<Dim + 1, VekCall>(var, end, fn);
        }
    }
}

} // namespace ndloop_detail

template <class Fn, class Int, std::size_t N>
__attribute__((always_inline)) inline auto
ndloop(vek<Int, N> const& begin, vek<Int, N> const& end, Fn const& fn)
    -> std::enable_if_t<std::is_invocable_v<Fn const&, vek<Int, N> const&>>
{
    vek<Int, N> var;
    ndloop_detail::for_all_helper<0, true>(var, begin, end, fn);
}

template <class Fn, class Int, std::size_t N>
__attribute__((always_inline)) inline auto
ndloop(vek<Int, N> const& begin, vek<Int, N> const& end, Fn const& fn)
    -> std::enable_if_t<ndloop_detail::is_exploded_invokable_v<Fn, vek<Int, N>>>
{
    vek<Int, N> var;
    ndloop_detail::for_all_helper<0, false>(var, begin, end, fn);
}

template <class Fn, class Int, std::size_t N>
__attribute__((always_inline)) inline auto ndloop(vek<Int, N> const& end,
                                                  Fn const&          fn)
    -> std::enable_if_t<std::is_invocable_v<Fn const&, vek<Int, N> const&>>
{
    vek<Int, N> var;
    ndloop_detail::for_all_helper<0, true>(var, end, fn);
}

template <class Fn, class Int, std::size_t N>
__attribute__((always_inline)) inline auto ndloop(vek<Int, N> const& end,
                                                  Fn const&          fn)
    -> std::enable_if_t<ndloop_detail::is_exploded_invokable_v<Fn, vek<Int, N>>>
{
    vek<Int, N> var;
    ndloop_detail::for_all_helper<0, false>(var, end, fn);
}

} // namespace sysml
