// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include <algorithm>   // for std::max
#include <cmath>       // for std::abs
#include <iterator>    // for std::advance
#include <type_traits> // for std::decay_t

namespace sysml
{

template <class LeftIt, class RightIt>
inline auto max_abs_difference(LeftIt lbegin, LeftIt lend,
                               RightIt rbegin) noexcept
    -> std::decay_t<decltype(*lbegin - *rbegin)>
{
    // TODO(zi) assert on iterator concepts

    using return_type = std::decay_t<decltype(*lbegin - *rbegin)>;

    return_type ret = 0; // assume implicitly convertible from 0

    for (; lbegin != lend; ++lbegin, ++rbegin)
    {
        ret = std::max(ret, std::abs(*lbegin - *rbegin));
    }

    return ret;
}

template <class LeftIt, class Size, class RightIt>
inline auto max_abs_difference_n(LeftIt lbegin, Size n, RightIt rbegin) noexcept
    -> std::decay_t<decltype(*lbegin - *rbegin)>
{
    // TODO(zi) assert on iterator concepts

    using return_type = std::decay_t<decltype(*lbegin - *rbegin)>;

    return_type ret = 0; // assume implicitly convertible from 0

    for (Size i = 0; i < n; ++i, ++lbegin, ++rbegin)
    {
        ret = std::max(ret, std::abs(*lbegin - *rbegin));
    }

    return ret;

    /* TODO(zi) decide on using an alternative implementation
       auto lend = lbegin;
       std::advance(lend, n);
       return max_abs_difference(lbegin, lend, rbegin);
    */
}

} // namespace sysml
