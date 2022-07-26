// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include <cassert> // for assert
#include <cstddef> // for std::size_t

namespace sysml::detail::osforest
{

struct node_base
{
    node_base* prev_        = nullptr;
    node_base* next_        = nullptr;
    node_base* parent_      = nullptr;
    node_base* first_child_ = nullptr;
    node_base* last_child_  = nullptr;
    // std::size_t num_children_ = 0;
};

[[nodiscard]] inline bool is_naked_node(node_base const* node) noexcept
{
    return ((node->parent_ == nullptr) && (node->prev_ == nullptr) &&
            (node->next_ == nullptr));
}

inline void insert_before_this_one(node_base* this_one,
                                   node_base* node) noexcept
{
    assert(this_one);
    assert(node);
    assert(is_naked_node(node)); // todo(ZI) strong_assert ??

    auto prev = this_one->prev_;

    node->parent_   = this_one->parent_;
    node->next_     = this_one;
    node->prev_     = this_one->prev_;
    this_one->prev_ = node;

    if (prev)
    {
        prev->next_ = node;
    }
    else if (node->parent_)
    {
        node->parent_->first_child_ = node;
    }

    // if (node->parent_)
    // {
    //     ++node->parent_->num_children_;
    // }
}

inline void insert_after_this_one(node_base* this_one, node_base* node) noexcept
{
    assert(this_one);
    assert(node);
    assert(is_naked_node(node)); // todo(ZI) strong_assert ??

    auto next = this_one->next_;

    node->parent_   = this_one->parent_;
    node->prev_     = this_one;
    node->next_     = this_one->next_;
    this_one->next_ = node;

    if (next)
    {
        next->prev_ = node;
    }
    else if (node->parent_)
    {
        node->parent_->last_child_ = node;
    }

    // if (node->parent_)
    // {
    //     ++node->parent_->num_children_;
    // }
}

inline void children_push_front(node_base* parent, node_base* node) noexcept
{
    assert(parent);
    assert(node && is_naked_node(node));

    if (parent->first_child_)
    {
        insert_before_this_one(parent->first_child_, node);
    }
    else
    {
        node->parent_        = parent;
        parent->first_child_ = parent->last_child_ = node;
    }
}

inline void children_push_back(node_base* parent, node_base* node) noexcept
{
    assert(parent);
    assert(node && is_naked_node(node));

    if (parent->last_child_)
    {
        insert_after_this_one(parent->last_child_, node);
    }
    else
    {
        node->parent_        = parent;
        parent->first_child_ = parent->last_child_ = node;
    }
}

inline node_base* extract_node(node_base* to_extract) noexcept
{
    assert(to_extract);

    if (to_extract->parent_)
    {
        auto parent = to_extract->parent_;

        if (to_extract->prev_ == nullptr)
        {
            parent->first_child_ = to_extract->next_;
        }

        if (to_extract->next_ == nullptr)
        {
            parent->last_child_ = to_extract->prev_;
        }
    }

    if (to_extract->prev_)
    {
        to_extract->prev_->next_ = to_extract->next_;
    }

    if (to_extract->next_)
    {
        to_extract->next_->prev_ = to_extract->prev_;
    }

    to_extract->prev_ = to_extract->next_ = to_extract->parent_ = nullptr;

    return to_extract;
}

} // namespace sysml::detail::osforest
