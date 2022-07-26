// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/osforest/detail/node.hpp"

#include <cassert>     // for assert
#include <type_traits> // for std::conditional, std::enable_if

namespace sysml::detail::osforest
{

template <class, bool>
class basic_navigator;

template <class T, bool IsConst>
class basic_navigator<rep_node<T>, IsConst>
{
private:
    using rep_node_type      = rep_node<T>;
    using rep_node_ptr       = rep_node_type*;
    using rep_node_const_ptr = rep_node_type const*;

    using stored_rep_node_ptr =
        std::conditional_t<IsConst, rep_node_const_ptr, rep_node_ptr>;

    using value_type      = T;
    using reference       = T&;
    using const_reference = T const&;

    using returned_reference_type =
        std::conditional_t<IsConst, const_reference, reference>;

private:
    stored_rep_node_ptr ptr_ = nullptr;

    // template<class, class>
    // friend class osforest;

public: // TODO(zi) should be private and friend
    constexpr stored_rep_node_ptr get_ptr() const noexcept { return ptr_; }

public:
    constexpr returned_reference_type get() const noexcept
    {
        return ptr_->get();
    }
    constexpr returned_reference_type operator*() const noexcept
    {
        return ptr_->get();
    }
    constexpr stored_rep_node_ptr operator->() const noexcept { return ptr_; }

public:
    constexpr basic_navigator() noexcept = default;
    ~basic_navigator()                   = default;

    constexpr basic_navigator(basic_navigator const&) noexcept = default;
    constexpr basic_navigator(basic_navigator&&) noexcept      = default;

    constexpr basic_navigator&
    operator=(basic_navigator const&) noexcept = default;
    constexpr basic_navigator& operator=(basic_navigator&&) noexcept = default;

    explicit constexpr basic_navigator(stored_rep_node_ptr r) noexcept
        : ptr_(r)
    {
    }

    template <class, bool>
    friend class basic_navigator;

    constexpr basic_navigator(
        basic_navigator<rep_node_type, !IsConst> const& other) noexcept
        requires(IsConst)
        : ptr_(other.ptr_)
    {
    }

    template <bool OIsConst>
    constexpr basic_navigator&
    operator=(basic_navigator<rep_node_type, OIsConst> const& other) noexcept
        requires(IsConst || !OIsConst)
    {
        ptr_ = other.ptr_;
        return *this;
    }

    template <bool OIsConst>
    constexpr bool operator==(
        basic_navigator<rep_node_type, OIsConst> const& other) const noexcept
    {
        return ptr_ == other.ptr_;
    }

    template <bool OIsConst>
    constexpr bool operator!=(
        basic_navigator<rep_node_type, OIsConst> const& other) const noexcept
    {
        return !(*this == other);
    }

    // constexpr operator basic_navigator<rep_node_type, true>() const noexcept
    //     requires(!IsConst)
    // {
    //     return basic_navigator<rep_node_type, true>(ptr_);
    // }

public :

    //
    explicit constexpr
    operator bool() const noexcept
    {
        return ptr_ != nullptr;
    }
    constexpr basic_navigator next() const noexcept
    {
        assert(ptr_);
        return basic_navigator(ptr_->next());
    }
    constexpr basic_navigator prev() const noexcept
    {
        assert(ptr_);
        return basic_navigator(ptr_->prev());
    }
    constexpr basic_navigator parent() const noexcept
    {
        assert(ptr_);
        return basic_navigator(ptr_->parent());
    }
    constexpr basic_navigator first_child() const noexcept
    {
        assert(ptr_);
        return basic_navigator(ptr_->first_child());
    }
    constexpr basic_navigator last_child() const noexcept
    {
        assert(ptr_);
        return basic_navigator(ptr_->last_child());
    }

    constexpr bool has_next() const noexcept { return ptr_ && ptr_->next(); }
    constexpr bool has_prev() const noexcept { return ptr_ && ptr_->prev(); }
    constexpr bool has_parent() const noexcept
    {
        return ptr_ && ptr_->parent();
    }
    constexpr bool has_first_child() const noexcept
    {
        return ptr_ && ptr_->first_child();
    }
    constexpr bool has_last_child() const noexcept
    {
        return ptr_ && ptr_->last_child();
    }

    constexpr basic_navigator& move_to_next() noexcept
    {
        assert(ptr_);
        ptr_ = ptr_->next();
        return *this;
    }
    constexpr basic_navigator& move_to_prev() noexcept
    {
        assert(ptr_);
        ptr_ = ptr_->prev();
        return *this;
    }
    constexpr basic_navigator& move_to_parent() noexcept
    {
        assert(ptr_);
        ptr_ = ptr_->parent();
        return *this;
    }
    constexpr basic_navigator& move_to_first_child() noexcept
    {
        assert(ptr_);
        ptr_ = ptr_->first_child();
        return *this;
    }
    constexpr basic_navigator& move_to_last_child() noexcept
    {
        assert(ptr_);
        ptr_ = ptr_->last_child();
        return *this;
    }
};

} // namespace sysml::detail::osforest
