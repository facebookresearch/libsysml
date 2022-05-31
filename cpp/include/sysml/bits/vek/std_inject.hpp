#pragma once

#include "sysml/bits/vek/vek_struct.hpp"

#include <cstddef>     // for std::size_t
#include <tuple>       // for std::get
#include <type_traits> // for std::integral_constant
#include <utility>     // for std::move

// Specializations for the stdandard Library

template <typename Tp, std::size_t Nm>
struct std::tuple_size<sysml::vek<Tp, Nm>>
    : std::integral_constant<std::size_t, Nm>
{
};

template <std::size_t Idx, typename Tp, std::size_t Nm>
struct std::tuple_element<Idx, sysml::vek<Tp, Nm>>
{
    static_assert(Idx < Nm);
    using type = Tp;
};
