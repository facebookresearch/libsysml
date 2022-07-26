// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/osforest/detail/node_base.hpp"

namespace sysml::detail::osforest
{

template <class T>
struct node_impl : node_base
{
    T&       self() noexcept { return static_cast<T&>(*this); }
    T const& self() const noexcept { return static_cast<T const&>(*this); }

    T*       this_ptr() noexcept { return static_cast<T*>(this); }
    T const* this_ptr() const noexcept { return static_cast<T const*>(this); }

    T* next() noexcept { return static_cast<T*>(this->next_); }
    T* prev() noexcept { return static_cast<T*>(this->prev_); }
    T* parent() noexcept { return static_cast<T*>(this->parent_); }
    T* first_child() noexcept { return static_cast<T*>(this->first_child_); }
    T* last_child() noexcept { return static_cast<T*>(this->last_child_); }

    T const* next() const noexcept
    {
        return static_cast<T const*>(this->next_);
    }

    T const* prev() const noexcept
    {
        return static_cast<T const*>(this->prev_);
    }

    T const* parent() const noexcept
    {
        return static_cast<T const*>(this->parent_);
    }

    T const* first_child() const noexcept
    {
        return static_cast<T const*>(this->first_child_);
    }

    T const* last_child() const noexcept
    {
        return static_cast<T const*>(this->last_child_);
    }
};

template <class T>
class rep_node : public node_impl<rep_node<T>>
{
private:
    T value_;

    rep_node(rep_node const&) = delete;
    rep_node(rep_node&&)      = delete;

    rep_node& operator=(rep_node const&) = delete;
    rep_node& operator=(rep_node&&) = delete;

public:
    rep_node()  = default;
    ~rep_node() = default;

    template <class... Ts>
    rep_node(Ts&&... vals)
        : value_(std::forward<Ts>(vals)...)
    {
    }

    T&       get() { return value_; }
    T const& get() const { return value_; }
};

} // namespace sysml::detail::osforest
