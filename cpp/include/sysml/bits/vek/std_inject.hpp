#pragma once

#include "sysml/bits/vek/vek_struct.hpp"

#include <cstddef>     // for std::size_t
#include <tuple>       // for std::get
#include <type_traits> // for std::integral_constant
#include <utility>     // for std::move

// Specializations for the stdandard Library

namespace std
{

template <std::size_t Idx, typename Tp, std::size_t Nm>
constexpr Tp& get(sysml::vek<Tp, Nm>& v) noexcept
{
    static_assert(Idx < Nm);
    return v[Idx];
}

template <std::size_t Idx, typename Tp, std::size_t Nm>
constexpr Tp&& get(sysml::vek<Tp, Nm>&& v) noexcept
{
    static_assert(Idx < Nm);
    return std::move(std::get<Idx>(v));
}

template <std::size_t Idx, typename Tp, std::size_t Nm>
constexpr Tp const& get(sysml::vek<Tp, Nm> const& v) noexcept
{
    static_assert(Idx < Nm);
    return v[Idx];
}

template <std::size_t Idx, typename Tp, std::size_t Nm>
constexpr Tp const&& get(sysml::vek<Tp, Nm> const&& v) noexcept
{
    static_assert(Idx < Nm);
    return std::move(std::get<Idx>(v));
}

template <typename>
struct tuple_size;

template <typename Tp, std::size_t Nm>
struct tuple_size<sysml::vek<Tp, Nm>> : std::integral_constant<std::size_t, Nm>
{
};

template <std::size_t, typename>
struct tuple_element;

template <std::size_t Idx, typename Tp, std::size_t Nm>
struct tuple_element<Idx, sysml::vek<Tp, Nm>>
{
    static_assert(Idx < Nm);
    using type = Tp;
};

} // namespace std
