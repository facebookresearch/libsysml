// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/bits/arithmetic.hpp" // for sysml::arithmetic

#include <algorithm>   // for std::fill_n, std::swap, std::swap_ranges
#include <array>       // for std::array
#include <cstddef>     // for std::size_t, std::ptrdiff_t
#include <iterator>    // for std::reverse_iterator
#include <stdexcept>   // for std::out_of_range
#include <type_traits> // for std::is_nothrow_swappable, std::enable_if

namespace sysml
{

template <arithmetic Tp, std::size_t Nm>
struct vek
{
    static_assert(Nm > 0, "Vek only supports non-zero sizes");

    Tp elems_sysml_private_var_[Nm];

    using value_type             = Tp;
    using pointer                = value_type*;
    using const_pointer          = value_type const*;
    using reference              = value_type&;
    using const_reference        = value_type const&;
    using iterator               = value_type*;
    using const_iterator         = value_type const*;
    using size_type              = std::size_t;
    using difference_type        = std::ptrdiff_t;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    constexpr pointer data() noexcept
    {
        return const_cast<pointer>(elems_sysml_private_var_);
    }
    constexpr const_pointer data() const noexcept
    {
        return const_cast<const_pointer>(elems_sysml_private_var_);
    }
    constexpr const_pointer cdata() const noexcept
    {
        return const_cast<const_pointer>(elems_sysml_private_var_);
    }

    // clang-format off

    // Iterators
    constexpr iterator       begin()        noexcept { return iterator(data()); }
    constexpr const_iterator begin()  const noexcept { return const_iterator(data()); }
    constexpr const_iterator cbegin() const noexcept { return const_iterator(data()); }
    constexpr iterator       end()          noexcept { return iterator(data() + Nm); }
    constexpr const_iterator end()    const noexcept { return const_iterator(data() + Nm); }
    constexpr const_iterator cend()   const noexcept { return const_iterator(data() + Nm); }

    constexpr reverse_iterator       rbegin()        noexcept { return reverse_iterator(end()); }
    constexpr const_reverse_iterator rbegin()  const noexcept { return const_reverse_iterator(end()); }
    constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
    constexpr reverse_iterator       rend()          noexcept { return reverse_iterator(begin()); }
    constexpr const_reverse_iterator rend()    const noexcept { return const_reverse_iterator(begin()); }
    constexpr const_reverse_iterator crend()   const noexcept { return const_reverse_iterator(begin()); }

    // Capacity
    constexpr size_type size() const noexcept { return Nm; }
    constexpr size_type max_size() const noexcept { return Nm; }
    [[nodiscard]]
    constexpr bool      empty() const noexcept { return size() == 0; }

    // Element access
    constexpr reference       operator[](size_type n)       noexcept { return elems_sysml_private_var_[n]; }
    constexpr const_reference operator[](size_type n) const noexcept { return elems_sysml_private_var_[n]; }

    constexpr reference       front()       noexcept { return elems_sysml_private_var_[0]; }
    constexpr const_reference front() const noexcept { return elems_sysml_private_var_[0]; }

    constexpr reference       back()       noexcept { return elems_sysml_private_var_[Nm - 1]; }
    constexpr const_reference back() const noexcept { return elems_sysml_private_var_[Nm - 1]; }

    // clang-format on

    constexpr reference at(size_type n)
    {
        if (n >= Nm)
        {
            throw std::out_of_range(
                "vec::at out of range"); // TODO(zi) provide size() and n;
        }
        return elems_sysml_private_var_[n];
    }

    constexpr const_reference at(size_type n) const
    {
        if (n >= Nm)
        {
            throw std::out_of_range(
                "vec::at out of range"); // TODO(zi) provide size() and n;
        }
        return elems_sysml_private_var_[n];
    }

    constexpr void fill(value_type const& v)
    {
        std::fill_n(begin(), size(), v);
    }

    constexpr void swap(vek& other) noexcept(std::is_nothrow_swappable_v<Tp>)
    {
        std::swap_ranges(begin(), end(), other.begin());
    }

#define SYSML_VEK_ASSIGNMENT_OR_COMPOUND_ASSIGNMENT_OPERATOR_DEFINITION(OPER)  \
    template <arithmetic U>                                                    \
    requires requires(Tp a, U b) { a OPER b; }                                 \
    constexpr vek& operator OPER(U v) noexcept                                 \
    {                                                                          \
        for (size_type i = 0; i < size(); ++i)                                 \
        {                                                                      \
            elems_sysml_private_var_[i] OPER v;                                \
        }                                                                      \
        return *this;                                                          \
    }                                                                          \
                                                                               \
    template <arithmetic U>                                                    \
    requires requires(Tp a, U b) { a OPER b; }                                 \
    constexpr vek& operator OPER(vek<U, Nm> const& v) noexcept                 \
    {                                                                          \
        for (size_type i = 0; i < size(); ++i)                                 \
        {                                                                      \
            elems_sysml_private_var_[i] OPER v[i];                             \
        }                                                                      \
        return *this;                                                          \
    }                                                                          \
                                                                               \
    template <arithmetic U>                                                    \
    requires requires(Tp a, U b) { a OPER b; }                                 \
    constexpr vek& operator OPER(std::array<U, Nm> const& v) noexcept          \
    {                                                                          \
        for (size_type i = 0; i < size(); ++i)                                 \
        {                                                                      \
            elems_sysml_private_var_[i] OPER v[i];                             \
        }                                                                      \
        return *this;                                                          \
    }

    SYSML_VEK_ASSIGNMENT_OR_COMPOUND_ASSIGNMENT_OPERATOR_DEFINITION(+=)
    SYSML_VEK_ASSIGNMENT_OR_COMPOUND_ASSIGNMENT_OPERATOR_DEFINITION(-=)
    SYSML_VEK_ASSIGNMENT_OR_COMPOUND_ASSIGNMENT_OPERATOR_DEFINITION(*=)
    SYSML_VEK_ASSIGNMENT_OR_COMPOUND_ASSIGNMENT_OPERATOR_DEFINITION(/=)
    SYSML_VEK_ASSIGNMENT_OR_COMPOUND_ASSIGNMENT_OPERATOR_DEFINITION(%=)
    SYSML_VEK_ASSIGNMENT_OR_COMPOUND_ASSIGNMENT_OPERATOR_DEFINITION(&=)
    SYSML_VEK_ASSIGNMENT_OR_COMPOUND_ASSIGNMENT_OPERATOR_DEFINITION(|=)
    SYSML_VEK_ASSIGNMENT_OR_COMPOUND_ASSIGNMENT_OPERATOR_DEFINITION(^=)
    SYSML_VEK_ASSIGNMENT_OR_COMPOUND_ASSIGNMENT_OPERATOR_DEFINITION(<<=)
    SYSML_VEK_ASSIGNMENT_OR_COMPOUND_ASSIGNMENT_OPERATOR_DEFINITION(>>=)
    SYSML_VEK_ASSIGNMENT_OR_COMPOUND_ASSIGNMENT_OPERATOR_DEFINITION(=)

#undef SYSML_VEK_ASSIGNMENT_OR_COMPOUND_ASSIGNMENT_OPERATOR_DEFINITION

    vek& operator++() noexcept requires requires(Tp x) { ++x; }
    {
        for (size_type i = 0; i < size(); ++i)
        {
            ++elems_sysml_private_var_[i];
        }
        return *this;
    }

    template <std::size_t... Idx>
    vek postfix_plus_plus_private(std::index_sequence<Idx...>) noexcept
    {
        return {{elems_sysml_private_var_[Idx]++...}};
    }

    vek operator++(int) noexcept requires requires(Tp x) { x++; }
    {
        return postfix_plus_plus_private(std::make_index_sequence<Nm>{});
    }

    vek& operator--() noexcept requires requires(Tp x) { --x; }
    {
        for (size_type i = 0; i < size(); ++i)
        {
            --elems_sysml_private_var_[i];
        }
        return *this;
    }

    template <std::size_t... Idx>
    vek postfix_minus_minus_private(std::index_sequence<Idx...>) noexcept
    {
        return {{elems_sysml_private_var_[Idx]--...}};
    }

    vek operator--(int) noexcept requires requires(Tp x) { x--; }
    {
        return postfix_minus_minus_private(std::make_index_sequence<Nm>{});
    }
};

#if __cpp_deduction_guides >= 201606
template <typename Tp, typename... TRest>
vek(Tp, TRest...)
    -> vek<std::enable_if_t<(std::is_same_v<Tp, TRest> && ...), Tp>,
           1 + sizeof...(TRest)>;
#endif

} // namespace sysml
