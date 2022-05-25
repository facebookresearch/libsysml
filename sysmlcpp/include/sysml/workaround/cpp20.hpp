// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#if defined(__clang__) || defined(__GNUC__)

#    if (__cplusplus >= 202002) // Have c++ 20 support

#        include <bit>         // for bit_cast
#        include <type_traits> // for type_identity

#    else

#        include <type_traits>

namespace std
{

template <class T>
struct type_identity
{
    using type = T;
};

template <class T>
using type_identity_t = typename type_identity<T>::type;

// Sourced from https://en.cppreference.com/w/cpp/numeric/bit_cast
// to enable bit_cast from C++20
template <class To, class From>
typename std::enable_if_t<sizeof(To) == sizeof(From) &&
                              std::is_trivially_copyable_v<From> &&
                              std::is_trivially_copyable_v<To>,
                          To>
// constexpr support needs compiler magic
bit_cast(const From& src) noexcept
{
    static_assert(std::is_trivially_constructible_v<To>,
                  "This implementation additionally requires destination type "
                  "to be trivially constructible");

    To dst;
    std::memcpy(&dst, &src, sizeof(To));
    return dst;
}

} // namespace std

#    endif

#else

#    error "Unsupported compier for now"

#endif
