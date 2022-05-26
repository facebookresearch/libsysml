// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/numeric/integer.hpp"
#include "sysml/type_traits.hpp"

#include <cassert>
#include <cstddef>

namespace sysml
{

template <unsigned ElementSize, bool IsSigned, unsigned NumElements>
class alignas((ElementSize) * (NumElements)) ivec
{
private:
    static constexpr bool is_powerof2(unsigned v)
    {
        return v && ((v & (v - 1)) == 0);
    }

    static constexpr unsigned total_size = ElementSize * NumElements;
    static_assert(is_powerof2(total_size) && total_size <= 8);

public:
    using value_type      = integer_type_t<ElementSize, IsSigned>;
    using underlying_type = integer_type_t<total_size, false>;

private:
    union
    {
        underlying_type underlying;
        value_type      values[NumElements];
    } data_;

public:
    constexpr ivec() = default;

    template <class... Ts>
    constexpr ivec(Ts... vs)
    {
        static_assert(all_are_same_v<Ts...> && sizeof...(Ts) == NumElements);
        unsigned j = 0;
        for (auto v : {vs...})
        {
            data_.values[j++] = v;
        }
    }

    constexpr ivec& operator=(ivec const&) = default;
    constexpr ivec& operator=(ivec&&) = default;

    constexpr ivec(ivec const&) = default;
    constexpr ivec(ivec&&)      = default;

    constexpr value_type& operator[](std::size_t i)
    {
        assert(i < NumElements);
        return data_.values[NumElements - i - 1];
    }

    constexpr value_type const& operator[](std::size_t i) const
    {
        assert(i < NumElements);
        return data_.values[NumElements - i - 1];
    }

    constexpr integer_type_t<ElementSize * NumElements, IsSigned>
    extended_element(std::size_t i) const
    {
        assert(i < NumElements);
        return data_.values[NumElements - i - 1];
    }

    constexpr underlying_type& underlying() { return data_.underlying; }

    constexpr underlying_type const& underlying() const
    {
        return data_.underlying;
    }

    template <bool OtherSigned>
    constexpr ivec& operator+=(ivec<ElementSize, OtherSigned, NumElements> rhs)
    {
        for (unsigned i = 0; i < NumElements; ++i)
        {
            (*this)[i] += rhs[i];
        }
        return *this;
    }

    template <bool OtherSigned>
    constexpr ivec& operator-=(ivec<ElementSize, OtherSigned, NumElements> rhs)
    {
        for (unsigned i = 0; i < NumElements; ++i)
        {
            (*this)[i] -= rhs[i];
        }
        return *this;
    }

    constexpr ivec operator+() const noexcept { return *this; }

    constexpr ivec operator-() const noexcept
    {
        ivec ret = *this;
        for (unsigned i = 0; i < NumElements; ++i)
        {
            ret[i] = -(*this)[i];
        }
        return ret;
    }
};

template <unsigned ElementSize, bool LHSSigned, bool RHSSigned,
          unsigned NumElements>
constexpr inline bool operator==(ivec<ElementSize, LHSSigned, NumElements> lhs,
                                 ivec<ElementSize, RHSSigned, NumElements> rhs)
{
    return lhs.underlying() == rhs.underlying();
}

template <unsigned ElementSize, bool LHSSigned, bool RHSSigned,
          unsigned NumElements>
constexpr inline bool operator!=(ivec<ElementSize, LHSSigned, NumElements> lhs,
                                 ivec<ElementSize, RHSSigned, NumElements> rhs)
{
    return lhs.underlying() != rhs.underlying();
}

template <unsigned ElementSize, bool LHSSigned, bool RHSSigned,
          unsigned NumElements>
constexpr inline ivec<ElementSize, LHSSigned && RHSSigned, NumElements>
operator+(ivec<ElementSize, LHSSigned, NumElements> lhs,
          ivec<ElementSize, RHSSigned, NumElements> rhs)
{
    ivec<ElementSize, LHSSigned && RHSSigned, NumElements> ret;
    for (unsigned i = 0; i < NumElements; ++i)
    {
        ret[i] = lhs[i] + rhs[i];
    }
    return ret;
}

template <unsigned ElementSize, bool LHSSigned, bool RHSSigned,
          unsigned NumElements>
constexpr inline ivec<ElementSize, LHSSigned && RHSSigned, NumElements>
operator-(ivec<ElementSize, LHSSigned, NumElements> lhs,
          ivec<ElementSize, RHSSigned, NumElements> rhs)
{
    ivec<ElementSize, LHSSigned && RHSSigned, NumElements> ret;
    for (unsigned i = 0; i < NumElements; ++i)
    {
        ret[i] = lhs[i] - rhs[i];
    }
    return ret;
}

template <unsigned ElementSize, bool LHSSigned, bool RHSSigned,
          unsigned NumElements>
constexpr inline integer_type_t<ElementSize * NumElements,
                                LHSSigned || RHSSigned>
operator*(ivec<ElementSize, LHSSigned, NumElements> lhs,
          ivec<ElementSize, RHSSigned, NumElements> rhs)
{
    using return_type =
        integer_type_t<ElementSize * NumElements, LHSSigned || RHSSigned>;
    return_type ret = 0;

    for (unsigned i = 0; i < NumElements; ++i)
    {
        ret +=
            static_cast<return_type>(lhs[i]) * static_cast<return_type>(rhs[i]);
    }
    return ret;
}

template <class CharT, class Traits, unsigned ElementSize, bool IsSigned,
          unsigned NumElements>
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>&       os,
           ivec<ElementSize, IsSigned, NumElements> v)
{
    os << (IsSigned ? "int" : "uint") << (ElementSize * 8) << 'x' << NumElements
       << "_t<";
    os << v.extended_element(0);

    for (unsigned i = 1; i < NumElements; ++i)
    {
        os << "," << v.extended_element(i);
    }
    os << ">";
    return os;
}

namespace ivec_specializations
{
using int8x2_t  = ivec<1, true, 2>;
using uint8x2_t = ivec<1, false, 2>;

using int8x4_t  = ivec<1, true, 4>;
using uint8x4_t = ivec<1, false, 4>;

using int8x8_t  = ivec<1, true, 8>;
using uint8x8_t = ivec<1, false, 8>;

using int16x2_t  = ivec<2, true, 2>;
using uint16x2_t = ivec<2, false, 2>;

using int16x4_t  = ivec<2, true, 4>;
using uint16x4_t = ivec<2, false, 4>;

using int32x2_t  = ivec<4, true, 2>;
using uint32x2_t = ivec<4, false, 2>;
} // namespace ivec_specializations

using namespace ivec_specializations;

} // namespace sysml
