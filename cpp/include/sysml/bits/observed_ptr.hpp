// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include <cstddef>     // for std::nullptr_t
#include <type_traits> // for std::add_lvalue_reference
#include <utility>     // for std::exchange, std::swap

namespace sysml
{

template <typename T>
class observed_ptr
{
public:
    using element_type = T;
    using pointer      = T*;

private:
    element_type* ptr_;

public: // Constructors
    constexpr observed_ptr() noexcept
        : ptr_(nullptr)
    {
    }

    constexpr observed_ptr(std::nullptr_t)
        : ptr_(nullptr)
    {
    }

    constexpr explicit observed_ptr(element_type* p)
        : ptr_(p)
    {
    }

    template <class U>
    constexpr observed_ptr(observed_ptr<U> other) noexcept
        : ptr_(other.get())
    {
    }

    observed_ptr(observed_ptr const&) = default;
    observed_ptr(observed_ptr&&)      = default;

public: // Destructor - imlicitly declared
    ~observed_ptr() = default;

public: // operator= - imlicitly declared
    observed_ptr& operator=(observed_ptr const&) = default;
    observed_ptr& operator=(observed_ptr&&) = default;

public: // Modifiers
    constexpr element_type* release() noexcept
    {
        return std::exchange(ptr_, nullptr);
    }

    constexpr void reset(element_type* p = nullptr) noexcept { ptr_ = p; }

    constexpr void swap(observed_ptr& other) noexcept
    {
        std::swap(ptr_, other.ptr_);
    }

public: // Observeds
    constexpr element_type* get() const noexcept { return ptr_; }

    constexpr explicit operator bool() const noexcept { return ptr_; }

    constexpr std::add_lvalue_reference_t<element_type> operator*() const
    {
        return *ptr_;
    }

    constexpr element_type* operator->() const noexcept { return ptr_; }

public: // Conversions
    constexpr explicit operator element_type*() const noexcept { return ptr_; }
};

template <class T>
constexpr observed_ptr<T> make_observed(T* p) noexcept
{
    return observed_ptr<T>(p);
}

template <class T1, class T2>
constexpr bool operator==(observed_ptr<T1> const& p1,
                          observed_ptr<T2> const& p2) noexcept
{
    return p1.get() == p2.get();
}

template <class T1, class T2>
constexpr bool operator!=(observed_ptr<T1> const& p1,
                          observed_ptr<T2> const& p2) noexcept
{
    return p1.get() != p2.get();
}

template <class T>
constexpr bool operator==(observed_ptr<T> const& p, std::nullptr_t) noexcept
{
    return p.get() == nullptr;
}

template <class T>
constexpr bool operator==(std::nullptr_t, observed_ptr<T> const& p) noexcept
{
    return p.get() == nullptr;
}

template <class T>
constexpr bool operator!=(observed_ptr<T> const& p, std::nullptr_t) noexcept
{
    return p.get() != nullptr;
}

template <class T>
constexpr bool operator!=(std::nullptr_t, observed_ptr<T> const& p) noexcept
{
    return p.get() != nullptr;
}

template <class T1, class T2>
constexpr bool operator<(observed_ptr<T1> const& p1, observed_ptr<T2> const& p2)
{
    return p1.get() < p2.get();
}

template <class T1, class T2>
constexpr bool operator>(observed_ptr<T1> const& p1, observed_ptr<T2> const& p2)
{
    return p1.get() > p2.get();
}

template <class T1, class T2>
constexpr bool operator<=(observed_ptr<T1> const& p1,
                          observed_ptr<T2> const& p2)
{
    return p1.get() <= p2.get();
}

template <class T1, class T2>
constexpr bool operator>=(observed_ptr<T1> const& p1,
                          observed_ptr<T2> const& p2)
{
    return p1.get() >= p2.get();
}

} // namespace sysml
