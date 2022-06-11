#pragma once

#include "sysml/bits/vek/vek_struct.hpp"

namespace sysml
{

template <arithmetic T, std::size_t N, arithmetic U>
constexpr auto dot(vek<T, N> const& lhs, vek<U, N> const& rhs) noexcept
    -> vek<decltype(std::declval<T>() * std::declval<U>()), N>
{
    auto ret = lhs[0] * rhs[0];
    for (std::size_t i = 1; i < N; ++i)
        ret += lhs[i] * rhs[i];
    return ret;
}

} // namespace sysml
