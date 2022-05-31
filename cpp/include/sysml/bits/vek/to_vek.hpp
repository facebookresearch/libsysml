#pragma once

#include "sysml/bits/vek/vek_struct.hpp"

#include <array>
#include <type_traits> // for std::remove_cv, std::is_nothrow_convertibe, std::enable_if, std::is_convertible
#include <utility>     // for std::index_sequence

namespace sysml
{

namespace vek_detail
{

template <arithmetic Tp, std::size_t... Idx>
constexpr vek<Tp, sizeof...(Idx)> to_vek_helper(Tp (&a)[sizeof...(Idx)],
                                                std::index_sequence<Idx...>)
{
    return {{a[Idx]...}};
}

template <arithmetic Tp, std::size_t... Idx>
constexpr vek<Tp, sizeof...(Idx)>
to_vek_helper(std::array<Tp, sizeof...(Idx)> const& a,
              std::index_sequence<Idx...>)
{
    return {{a[Idx]...}};
}

} // namespace vek_detail

template <arithmetic Tp, std::size_t Nm>
constexpr vek<Tp, Nm>
to_vek(Tp (&a)[Nm]) noexcept(std::is_nothrow_constructible_v<Tp, Tp&>)
{
    static_assert(std::is_constructible_v<Tp, Tp&>);

    return vek_detail::to_vek_helper(a, std::make_index_sequence<Nm>{});
}

template <arithmetic Tp, std::size_t Nm>
constexpr vek<Tp, Nm>
to_vek(Tp(&&a)[Nm]) noexcept(std::is_nothrow_constructible_v<Tp, Tp&>)
{
    static_assert(std::is_constructible_v<Tp, Tp&>);

    return vek_detail::to_vek_helper(a, std::make_index_sequence<Nm>{});
}

template <arithmetic Tp, std::size_t Nm>
constexpr vek<Tp, Nm> to_vek(std::array<Tp, Nm> const& a) noexcept(
    std::is_nothrow_constructible_v<Tp, Tp&>)
{
    static_assert(std::is_constructible_v<Tp, Tp&>);

    return vek_detail::to_vek_helper(a, std::make_index_sequence<Nm>{});
}

} // namespace sysml
