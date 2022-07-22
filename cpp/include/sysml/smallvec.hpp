// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/utility.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <memory>
#include <new>
#include <stdexcept>
#include <type_traits>

namespace sysml
{

template <class Tp, std::size_t Nm>
requires(std::is_nothrow_destructible_v<Tp>&& Nm > 0) class smallvec
{
private:
    static_assert(Nm > 0, "Smallvec only supports non-zero sizes");

public:
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

private:
    static const std::size_t value_type_size = sizeof(Tp);

    size_type size_ = 0;
    alignas(alignof(Tp)) std::byte buffer_[value_type_size * Nm];

private:
    template <bool CheckBounds = false>
    constexpr pointer pointer_to_element(
        std::size_t n,
        std::integral_constant<bool, CheckBounds> =
            std::integral_constant<bool, CheckBounds>()) noexcept(!CheckBounds)
    {
        if constexpr (CheckBounds)
        {
            if (n >= max_size())
            {
                throw std::out_of_range(
                    "smallvec access out of range"); // TODO(zi) provide size()
                                                     // and n;
            }
        }
        return std::launder(
            reinterpret_cast<pointer>(std::addressof(buffer_[n * sizeof(Tp)])));
    }

    template <bool CheckBounds = false>
    constexpr const_pointer
    pointer_to_element(std::size_t n,
                       std::integral_constant<bool, CheckBounds> =
                           std::integral_constant<bool, CheckBounds>()) const
        noexcept(!CheckBounds)
    {
        if constexpr (CheckBounds)
        {
            if (n >= max_size())
            {
                throw std::out_of_range(
                    "smallvec access out of range"); // TODO(zi) provide size()
                                                     // and n;
            }
        }
        return std::launder(reinterpret_cast<const_pointer>(
            std::addressof(buffer_[n * sizeof(Tp)])));
    }

public:
    constexpr pointer data() noexcept { return pointer_to_element(0); }

    constexpr const_pointer data() const noexcept
    {
        return pointer_to_element(0);
    }

    constexpr const_pointer cdata() const noexcept
    {
        return pointer_to_element(0);
    }

    // clang-format off

    // Iterators
    constexpr iterator       begin()        noexcept { return iterator(data()); }
    constexpr const_iterator begin()  const noexcept { return const_iterator(data()); }
    constexpr const_iterator cbegin() const noexcept { return const_iterator(data()); }
    constexpr iterator       end()          noexcept { return iterator(data() + size()); }
    constexpr const_iterator end()    const noexcept { return const_iterator(data() + size()); }
    constexpr const_iterator cend()   const noexcept { return const_iterator(data() + size()); }

    constexpr reverse_iterator       rbegin()        noexcept { return reverse_iterator(end()); }
    constexpr const_reverse_iterator rbegin()  const noexcept { return const_reverse_iterator(end()); }
    constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
    constexpr reverse_iterator       rend()          noexcept { return reverse_iterator(begin()); }
    constexpr const_reverse_iterator rend()    const noexcept { return const_reverse_iterator(begin()); }
    constexpr const_reverse_iterator crend()   const noexcept { return const_reverse_iterator(begin()); }

    // Capacity
    constexpr size_type size() const noexcept { return size_; }
    constexpr size_type max_size() const noexcept { return Nm; }
    constexpr size_type capacity() const noexcept { return Nm; }
    [[nodiscard]]
    constexpr bool      empty() const noexcept { return size() == 0; }

    // Element access
    constexpr reference       operator[](size_type n)       noexcept { return *pointer_to_element(n); }
    constexpr const_reference operator[](size_type n) const noexcept { return *pointer_to_element(n); }

    constexpr reference       front()       noexcept { return *pointer_to_element(0); }
    constexpr const_reference front() const noexcept { return *pointer_to_element(0); }

    constexpr reference       back()       noexcept { return *pointer_to_element(size() - 1); }
    constexpr const_reference back() const noexcept { return *pointer_to_element(size() - 1); }

    // clang-format on

    constexpr void
    assign(size_type count,
           Tp const& value) noexcept(std::is_nothrow_copy_constructible_v<Tp>)
    {
        resize(0);
        while (size_ < count)
        {
            std::construct_at(pointer_to_element(size_++), value);
        }
    }

    template <class InputIt>
    constexpr void assign(InputIt first, InputIt last) noexcept(
        std::is_nothrow_copy_constructible_v<
            Tp>) requires(!std::is_integral_v<InputIt>)
    {
        resize(0);
        for (; first != last; ++first)
        {
            std::construct_at(pointer_to_element(size_++), *first);
        }
    }

    constexpr void assign(std::initializer_list<Tp> ilist) noexcept(
        std::is_nothrow_copy_constructible_v<Tp>)
    {
        assign(ilist.begin(), ilist.end());
    }

    constexpr reference at(size_type n)
    {
        return *pointer_to_element(n, std::true_type{});
    }

    constexpr const_reference at(size_type n) const
    {
        return *pointer_to_element(n, std::true_type{});
    }

    constexpr void swap(smallvec& other) noexcept(
        std::is_nothrow_swappable_v<Tp> &&
        (std::is_nothrow_move_constructible_v<Tp> ||
         std::is_nothrow_copy_constructible_v<
             Tp>)) requires(std::is_swappable_v<Tp> &&
                            (std::is_move_constructible_v<Tp> ||
                             std::is_copy_constructible_v<Tp>))
    {
        for (size_type i = 0; i < std::min(size_, other.size_); ++i)
        {
            std::swap(this->operator[](i), other[i]);
        }

        if (size_ < other.size_)
        {
            for (auto i = size_; i < other.size_; ++i)
            {
                if constexpr (std::is_nothrow_move_constructible_v<Tp>)
                {
                    std::construct_at(pointer_to_element(i),
                                      std::move(other[i]));
                }
                else
                {
                    std::construct_at(pointer_to_element(i), other[i]);
                }
            }
            other.resize(std::exchange(size_, other.size_));
        }
        else
        {
            for (auto i = other.size_; i < size_; ++i)
            {
                if constexpr (std::is_nothrow_move_constructible_v<Tp>)
                {
                    std::construct_at(other->pointer_to_element(i),
                                      std::move(this->operator[](i)));
                }
                else
                {
                    std::construct_at(other->pointer_to_element(i),
                                      this->operator[](i));
                }
            }
            resize(std::exchange(other.size_, size_));
        }
    }

    void clear() noexcept
    {
        while (size_)
        {
            std::destroy_at(pointer_to_element(--size_));
        }
    }

    // Constructors
    constexpr smallvec() noexcept
        : size_(0)
    {
    }
    constexpr smallvec(size_type N) noexcept(
        std::is_nothrow_default_constructible_v<Tp>)
        : size_(0)
    {
        resize(N);
    }
    ~smallvec() { clear(); }

    constexpr smallvec(smallvec const& other) noexcept(
        std::is_nothrow_copy_constructible_v<Tp>)
        : size_(0)
    {
        if (this != std::addressof(other))
        {
            for (; size_ < other.size_; ++size_)
            {
                std::construct_at(pointer_to_element(size_), other[size_]);
            }
        }
    }

    constexpr smallvec& operator=(smallvec const& other) noexcept(
        std::is_nothrow_copy_constructible_v<Tp>)
    {
        if (this != std::addressof(other))
        {
            clear();
            for (; size_ < other.size_; ++size_)
            {
                std::construct_at(pointer_to_element(size_), other[size_]);
            }
        }
        return *this;
    }

    constexpr smallvec(smallvec&& other) noexcept(
        std::is_nothrow_move_constructible_v<Tp>)
        : size_(0)
    {
        if (this != std::addressof(other))
        {
            for (; size_ < other.size_; ++size_)
            {
                std::construct_at(pointer_to_element(size_),
                                  std::move(other[size_]));
            }
            other.clear();
        }
    }

    // todo(zi) call operator= on elements when present in both this and other
    constexpr smallvec& operator=(smallvec&& other) noexcept(
        std::is_nothrow_move_constructible_v<Tp>)
    {
        if (this != std::addressof(other))
        {
            if (this != std::addressof(other))
            {
                clear();
                for (; size_ < other.size_; ++size_)
                {
                    std::construct_at(pointer_to_element(size_),
                                      std::move(other[size_]));
                }
                other.clear();
            }
        }
        return *this;
    }

    constexpr void push_back(Tp const& value)
    {
        if (size_ >= max_size())
        {
            throw std::out_of_range("..."); // TODO(zi) provide size() and n;
        }

        std::construct_at(pointer_to_element(size_++), value);
    }

    constexpr void push_back(Tp&& value)
    {
        if (size_ >= max_size())
        {
            throw std::out_of_range("..."); // TODO(zi) provide size() and n;
        }

        std::construct_at(pointer_to_element(size_++), std::move(value));
    }

    template <class... Args>
    constexpr reference emplace_back(Args&&... args)
    {
        if (size_ >= max_size())
        {
            throw std::out_of_range("..."); // TODO(zi) provide size() and n;
        }

        std::construct_at(pointer_to_element(size_++),
                          std::forward<Args>(args)...);

        return back();
    }

    constexpr void pop_back() { std::destroy_at(pointer_to_element(--size_)); }

    constexpr void resize(size_type count)
    {
        if (count > max_size())
        {
            throw std::out_of_range("..."); // TODO(zi) provide size() and n;
        }

        for (; size_ < count; ++size_)
        {
            std::construct_at(pointer_to_element(size_));
        }

        for (; size_ > count; --size_)
        {
            std::destroy_at(pointer_to_element(size_ - 1));
        }
    }

    constexpr void resize(size_type count, value_type const& value)
    {
        if (count > max_size())
        {
            throw std::out_of_range("..."); // TODO(zi) provide size() and n;
        }

        for (; size_ < count; ++size_)
        {
            std::construct_at(pointer_to_element(size_), value);
        }

        for (; size_ > count; --size_)
        {
            std::destroy_at(pointer_to_element(size_ - 1));
        }
    }
};

} // namespace sysml
