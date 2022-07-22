// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/vek/vek_struct.hpp"

#include <ios>
#include <iosfwd>
#include <limits>
#include <string_view>
#include <tuple>

namespace sysml
{

namespace vek_detail
{
template <class V, class CharT>
struct vek_formating_helper;

template <arithmetic Tp, std::size_t Nm, class CharT>
struct vek_formating_helper<vek<Tp, Nm> const&, CharT>
{
    static constexpr auto               vek_len = Nm;
    vek<Tp, Nm> const&                  v;
    std::basic_string_view<CharT> const pre;
    std::basic_string_view<CharT> const separator;
    std::basic_string_view<CharT> const post;
};

template <arithmetic Tp, std::size_t Nm, class CharT>
struct vek_formating_helper<vek<Tp, Nm>&, CharT>
{
    static constexpr auto               vek_len = Nm;
    vek<Tp, Nm>&                        v;
    std::basic_string_view<CharT> const pre;
    std::basic_string_view<CharT> const separator;
    std::basic_string_view<CharT> const post;
};

template <class CharT, class CharTTraits, class Vek>
std::basic_ostream<CharT, CharTTraits>&
operator<<(std::basic_ostream<CharT, CharTTraits>& os,
           vek_formating_helper<Vek, CharT>&&      v)
{
    os << v.pre;
    if constexpr (vek_formating_helper<Vek, CharT>::vek_len > 0)
    {
        os << v.v[0];
        for (std::size_t i = 1; i < vek_formating_helper<Vek, CharT>::vek_len;
             ++i)
        {
            os << v.separator << v.v[i];
        }
    }
    os << v.post;
    return os;
}

template <class CharT, class CharTTraits>
bool match_string(std::basic_istream<CharT, CharTTraits>&           is,
                  std::basic_string_view<CharT, CharTTraits> const& str)
{
    typename std::basic_istream<CharT, CharTTraits>::sentry s(is);

    for (std::size_t i = 0; i < str.size(); ++i)
    {
        if (std::isspace(str[i], is.getloc())) continue;
        if (!is.good()) return false;

        auto c = is.get();
        if (c != str[i])
        {
            is.setstate(std::ios_base::failbit);
            return false;
        }
    }
    return true;
}

template <class CharT, class CharTTraits, class Vek>
std::basic_istream<CharT, CharTTraits>&
operator>>(std::basic_istream<CharT, CharTTraits>& is,
           vek_formating_helper<Vek, CharT>&&      v)
{
    typename std::basic_istream<CharT, CharTTraits>::sentry s(is);

    if (!match_string(is, v.pre)) return is;

    for (std::size_t idx = 0; idx < vek_formating_helper<Vek, CharT>::vek_len;
         ++idx)
    {
        if ((idx > 0) && !match_string(is, v.separator)) return is;
        if (!(is >> v.v[idx])) return is;
    }

    return is;
}

} // namespace vek_detail

template <class CharT, class CharTTraits, arithmetic Tp, std::size_t Nm>
std::basic_istream<CharT, CharTTraits>&
operator>>(std::basic_istream<CharT, CharTTraits>& is, vek<Tp, Nm>& v)
{

    if constexpr (Nm > 0)
    {
        typename std::basic_istream<CharT, CharTTraits>::sentry s(is);
        if (s)
        {
            for (std::size_t idx = 0; idx < Nm; ++idx)
            {
                if (!is.good())
                {
                    return is;
                }
                if (idx > 0)
                {
                    is.ignore(std::numeric_limits<std::streamsize>::max(), ',');
                    if (!is.good())
                    {
                        return is;
                    }
                }
                is >> v[idx];
            }
        }
    }
    return is;
}

template <class CharT, class CharTTraits, arithmetic Tp, std::size_t Nm>
std::basic_ostream<CharT, CharTTraits>&
operator<<(std::basic_ostream<CharT, CharTTraits>& os, vek<Tp, Nm> const& v)
{
    if constexpr (Nm > 0)
    {
        os << v[0];
        for (std::size_t i = 1; i < Nm; ++i)
        {
            os << ", " << v[i];
        }
    }
    return os;
}

template <arithmetic Tp, std::size_t Nm, class CharT>
constexpr vek_detail::vek_formating_helper<vek<Tp, Nm>&, CharT>
formatted(vek<Tp, Nm>& v, CharT const* pre = "<", CharT const* separator = ", ",
          CharT const* post = ">") noexcept
{
    return {v, pre, separator, post};
}

template <arithmetic Tp, std::size_t Nm, class CharT>
constexpr vek_detail::vek_formating_helper<vek<Tp, Nm> const&, CharT>
formatted(vek<Tp, Nm> const& v, CharT const* pre = "<",
          CharT const* separator = ", ", CharT const* post = ">") noexcept
{
    return {v, pre, separator, post};
}

template <arithmetic Tp, std::size_t Nm, class CharT>
constexpr vek_detail::vek_formating_helper<vek<Tp, Nm>&, CharT>
formatted(vek<Tp, Nm>&& v, CharT const* pre = "<",
          CharT const* separator = ", ", CharT const* post = ">") noexcept
{
    return {v, pre, separator, post};
}

template <arithmetic Tp, std::size_t Nm, class CharT>
constexpr vek_detail::vek_formating_helper<vek<Tp, Nm> const&, CharT>
formatted(vek<Tp, Nm> const&& v, CharT const* pre = "<",
          CharT const* separator = ", ", CharT const* post = ">") noexcept
{
    return {v, pre, separator, post};
}

} // namespace sysml
