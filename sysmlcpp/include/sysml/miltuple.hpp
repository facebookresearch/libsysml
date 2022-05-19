// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include <tuple>

namespace sysml
{

namespace detail
{

template <std::size_t N, class T, class... Args>
struct miltuple_builder
{
    using type = typename miltuple_builder<N - 1, T, T, Args...>::type;
};

template <class T, class... Args>
struct miltuple_builder<0, T, Args...>
{
    using type = std::tuple<Args...>;
};

template <class T, std::size_t N>
struct miltuple_creator
{
    using type = typename miltuple_builder<N, T>::type;
};

} // namespace detail

template <class T, std::size_t N>
using miltuple = typename detail::miltuple_creator<T, N>::type;

} // namespace sysml
