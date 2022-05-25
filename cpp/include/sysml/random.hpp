// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/numeric.hpp"
#include "sysml/type_traits.hpp"

#include <random>
#include <type_traits>

namespace sysml
{

namespace detail
{

template <class T>
class uniform_int_distribution : std::uniform_int_distribution<T>
{
private:
    using super_type = std::uniform_int_distribution<T>;

public:
    using super_type::super_type;
    using super_type::operator();
};

template <class T>
class uniform_real_distribution : std::uniform_real_distribution<T>
{
private:
    using super_type = std::uniform_real_distribution<T>;

public:
    using super_type::super_type;
    using super_type::operator();
};

// template <class>
// class uniform_ivec_distribution;

// template <unsigned ElementSize, bool IsSigned, unsigned NumElements>
// class uniform_ivec_distribution<ivec<ElementSize, IsSigned, NumElements>>
// {
// private:
//     using result_type =
//         uniform_ivec_distribution<ivec<ElementSize, IsSigned, NumElements>>;

//     using value_type = typename result_type::value_type;

//     std::uniform_int_distribution<value_type> underlying_;
//     // value_type low_, high_;

// public:
//     uniform_ivec_distribution(value_type low, value_type high)
//         : underlying_(low, high)
//     {
//     }

//     template <class Generator>
//     result_type operator()(Generator& g)
//     {
//         result_type ret;
//         for (unsigned i = 0; i < NumElements; ++i)
//         {
//             ret[i] = underlying_(g);
//         }
//         return ret;
//     }
// };

} // namespace detail

// template <class, class>
// class uniform_distribution_version;

// // clang-format off
// struct integral_uniform_distribution_tag{};
// struct real_uniform_distribution_tag{};
// struct ivec_uniform_distribution_tag{};
// // clang-format on

// } // namespace detail

template <class T>
using uniform_distribution = std::conditional_t<
    is_any_of_v<T, short, int, long, long long, unsigned short, unsigned int,
                unsigned long, unsigned long long>,
    detail::uniform_int_distribution<T>,
    std::conditional_t<is_any_of_v<T, float, double, long double>,
                       detail::uniform_real_distribution<T>, std::nullptr_t>>;

} // namespace sysml
