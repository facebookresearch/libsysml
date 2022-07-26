// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/osforest/detail/navigator.hpp"
#include "sysml/osforest/detail/node.hpp"
#include "sysml/utility.hpp"

#include <initializer_list> // for std::initializer_list
#include <memory>           // for std::allocator, std::allocator_traits

namespace sysml
{

template <class T, class Allocator = std::allocator<T>>
class osforest
{
private:
    using allocator_traits = std::allocator_traits<Allocator>;
    using rep_node_type    = detail::osforest::rep_node<T>;
    using rep_allocator_type =
        typename allocator_traits::template rebind_alloc<rep_node_type>;
    using rep_allocator_traits =
        typename allocator_traits::template rebind_traits<rep_node_type>;

public:
    using value_type      = T;
    using allocator_type  = Allocator;
    using reference       = value_type&;
    using const_reference = value_type const&;
    using size_type       = typename allocator_traits::size_type;
    using difference_type = typename allocator_traits::difference_type;
    using pointer         = typename allocator_traits::pointer;
    using const_pointer   = typename allocator_traits::const_pointer;

    using navigator = detail::osforest::basic_navigator<rep_node_type, false>;
    using const_navigator =
        detail::osforest::basic_navigator<rep_node_type, true>;

public:
private:
    rep_allocator_type alloc_{};
    rep_node_type*     rep_first_root_ = nullptr;
    rep_node_type*     rep_last_root_  = nullptr;

    static constexpr size_type recursive_destroy_rep(rep_allocator_type& a,
                                                     rep_node_type*      n);

    template <class... Args>
    static constexpr rep_node_type* make_rep_node(rep_allocator_type&,
                                                  Args&&...);

    static constexpr rep_node_type*
    rep_node_deep_copy_node(rep_allocator_type&, rep_node_type const*);

    static constexpr rep_node_type*
    rep_node_deep_copy_from_node(rep_allocator_type&, rep_node_type const*,
                                 rep_node_type*);

public:
    osforest() noexcept = default;
    explicit osforest(Allocator const& alloc) noexcept
        : alloc_(alloc)
        , rep_first_root_(nullptr)
        , rep_last_root_(nullptr)
    {
    }

    template <class InputIt>
    osforest(InputIt first, InputIt last, Allocator const& alloc = Allocator());

    osforest(osforest const& other);
    // TODO(maybe)
    // osforest(osforest const& other, Allocator const& alloc);

    osforest(osforest&& other);
    // TODO(maybe)
    // osforest(osforest&& other, Allocator const& alloc);

    // TODO(maybe)
    // osforest(std::initializer_list<T> init,
    //          Allocator const&         alloc = Allocator());

    osforest& operator=(osforest const& other);
    osforest& operator=(osforest&& other);

    ~osforest();

public:
    // TODO(remove)
    size_type size() const noexcept { return 0; }

    constexpr bool has_roots() const noexcept
    {
        assert(((rep_first_root_ == nullptr) && (rep_last_root_ == nullptr)) ||
               ((rep_first_root_ != nullptr) && (rep_last_root_ != nullptr)));

        return rep_first_root_;
    }

    constexpr osforest subforest(const_navigator const& from,
                                 const_navigator const& to) const
    {
        osforest ret(alloc_);

        for (const_navigator nav = from; nav != to; nav.move_to_next())
        {
            ret.roots_push_back(
                rep_node_deep_copy_node(ret.alloc_, nav.get_ptr()));
        }

        return ret;
    }

    constexpr osforest subforest(const_navigator const& at) const
    {
        osforest ret(alloc_);

        if (at)
        {
            ret.roots_push_back(
                rep_node_deep_copy_node(ret.alloc_, at.get_ptr()));
        }

        return ret;
    }

private:
    constexpr navigator roots_push_front(rep_node_type* n) noexcept
    {
        if (rep_first_root_)
        {
            rep_first_root_->prev_ = n;
            n->next_               = std::exchange(rep_first_root_, n);
        }
        else
        {
            rep_first_root_ = rep_last_root_ = n;
        }

        return navigator(rep_first_root_);
    }

    constexpr navigator roots_push_back(rep_node_type* n) noexcept
    {
        if (rep_last_root_)
        {
            rep_last_root_->next_ = n;
            n->prev_              = std::exchange(rep_last_root_, n);
        }
        else
        {
            rep_first_root_ = rep_last_root_ = n;
        }

        return navigator(rep_first_root_);
    }

    constexpr navigator insert_after(navigator const& nav,
                                     rep_node_type*   n) noexcept
    {
        assert(nav);
        auto this_one = nav.get_ptr();
        detail::osforest::insert_after_this_one(this_one, n);
        if (this_one == rep_last_root_)
        {
            rep_last_root_ = n;
        }
        return navigator(n);
    }

    constexpr navigator insert_before(navigator const& nav,
                                      rep_node_type*   n) noexcept
    {
        assert(nav);
        auto this_one = nav.get_ptr();
        detail::osforest::insert_before_this_one(this_one, n);
        if (this_one == rep_first_root_)
        {
            rep_first_root_ = n;
        }
        return navigator(n);
    }

    constexpr navigator roots_insert_after(navigator const& nav,
                                           rep_node_type*   n) noexcept
    {
        if (nav)
        {
            return insert_after(nav, n);
        }
        else
        {
            assert(!has_roots());
            rep_first_root_ = rep_last_root_ = n;
            return navigator(n);
        }
    }

    constexpr navigator roots_insert_before(navigator const& nav,
                                            rep_node_type*   n) noexcept
    {
        if (nav)
        {
            return insert_after(nav, n);
        }
        else
        {
            if (has_roots())
            {
                return insert_after(navigator(rep_last_root_), n);
            }
            else
            {
                rep_first_root_ = rep_last_root_ = n;
                return navigator(n);
            }
        }
    }

    constexpr navigator children_push_front(navigator const& nav,
                                            rep_node_type*   n) noexcept
    {
        assert(nav);
        auto this_one = nav.get_ptr();
        detail::osforest::children_push_front(this_one, n);
        return navigator(n);
    }

    constexpr navigator children_push_back(navigator const& nav,
                                           rep_node_type*   n) noexcept
    {
        assert(nav);
        auto this_one = nav.get_ptr();
        detail::osforest::children_push_back(this_one, n);
        return navigator(n);
    }

public:
    constexpr navigator roots_push_front(T const& val)
    {
        return roots_push_front(make_rep_node(alloc_, val));
    }

    constexpr navigator roots_push_front(T&& val)
    {
        return roots_push_front(make_rep_node(alloc_, std::move(val)));
    }

    template <class... Args>
    constexpr navigator roots_emplace_front(Args&&... args)
    {
        return roots_push_front(
            make_rep_node(alloc_, std::forward<Args>(args)...));
    }

    constexpr navigator roots_push_back(T const& val)
    {
        return roots_push_back(make_rep_node(alloc_, val));
    }

    constexpr navigator roots_push_back(T&& val)
    {
        return roots_push_back(make_rep_node(alloc_, std::move(val)));
    }

    template <class... Args>
    constexpr navigator roots_emplace_back(Args&&... args)
    {
        return roots_push_back(
            make_rep_node(alloc_, std::forward<Args>(args)...));
    }

    constexpr navigator insert_after(navigator const& nav, T const& val)
    {
        return insert_after(nav, make_rep_node(alloc_, val));
    }

    constexpr navigator insert_after(navigator const& nav, T&& val)
    {
        return insert_after(nav, make_rep_node(alloc_, std::move(val)));
    }

    template <class... Args>
    constexpr navigator emplace_after(navigator const& nav, Args&&... args)
    {
        return insert_after(nav,
                            make_rep_node(alloc_, std::forward<Args>(args)...));
    }

    constexpr navigator insert_before(navigator const& nav, T const& val)
    {
        return insert_before(nav, make_rep_node(alloc_, val));
    }

    constexpr navigator insert_before(navigator const& nav, T&& val)
    {
        return insert_before(nav, make_rep_node(alloc_, std::move(val)));
    }

    template <class... Args>
    constexpr navigator emplace_before(navigator const& nav, Args&&... args)
    {
        return insert_before(
            nav, make_rep_node(alloc_, std::forward<Args>(args)...));
    }

    constexpr navigator roots_insert_after(navigator const& nav, T const& val)
    {
        return roots_insert_after(nav, make_rep_node(alloc_, val));
    }

    constexpr navigator roots_insert_after(navigator const& nav, T&& val)
    {
        return roots_insert_after(nav, make_rep_node(alloc_, std::move(val)));
    }

    template <class... Args>
    constexpr navigator roots_emplace_after(navigator const& nav,
                                            Args&&... args)
    {
        return roots_insert_after(
            nav, make_rep_node(alloc_, std::forward<Args>(args)...));
    }

    constexpr navigator roots_insert_before(navigator const& nav, T const& val)
    {
        return roots_insert_before(nav, make_rep_node(alloc_, val));
    }

    constexpr navigator roots_insert_before(navigator const& nav, T&& val)
    {
        return roots_insert_before(nav, make_rep_node(alloc_, std::move(val)));
    }

    template <class... Args>
    constexpr navigator roots_emplace_before(navigator const& nav,
                                             Args&&... args)
    {
        return roots_insert_before(
            nav, make_rep_node(alloc_, std::forward<Args>(args)...));
    }

    constexpr navigator children_push_front(navigator const& nav, T const& val)
    {
        return children_push_front(nav, make_rep_node(alloc_, val));
    }

    constexpr navigator children_push_front(navigator const& nav, T&& val)
    {
        return children_push_front(nav, make_rep_node(alloc_, std::move(val)));
    }

    template <class... Args>
    constexpr navigator children_emplace_front(navigator const& nav,
                                               Args&&... args)
    {
        return children_push_front(
            nav, make_rep_node(alloc_, std::forward<Args>(args)...));
    }

    constexpr navigator children_push_back(navigator const& nav, T const& val)
    {
        return children_push_back(nav, make_rep_node(alloc_, val));
    }

    constexpr navigator children_push_back(navigator const& nav, T&& val)
    {
        return children_push_back(nav, make_rep_node(alloc_, std::move(val)));
    }

    template <class... Args>
    constexpr navigator children_emplace_back(navigator const& nav,
                                              Args&&... args)
    {
        return children_push_back(
            nav, make_rep_node(alloc_, std::forward<Args>(args)...));
    }

    constexpr navigator roots_begin() noexcept
    {
        return navigator(rep_first_root_);
    }
    constexpr navigator roots_end() noexcept { return navigator(nullptr); }
    // constexpr navigator roots_rbegin() noexcept
    // {
    //     return navigator(rep_last_root_);
    // }
    constexpr const_navigator roots_begin() const noexcept
    {
        return const_navigator(rep_first_root_);
    }
    constexpr const_navigator roots_end() const noexcept
    {
        return const_navigator(nullptr);
    }
    // constexpr const_navigator roots_rbegin() const noexcept
    // {
    //     return const_navigator(rep_last_root_);
    // }
    constexpr const_navigator roots_cbegin() const noexcept
    {
        return const_navigator(rep_first_root_);
    }
    constexpr const_navigator roots_cend() const noexcept
    {
        return const_navigator(nullptr);
    }
    // constexpr const_navigator roots_crbegin() const noexcept
    // {
    //     return const_navigator(rep_last_root_);
    // }
};

} // namespace sysml

//
//
//

#include <utility> // for std::exchange

namespace sysml
{

template <class T, class Allocator>
template <class... Args>
constexpr typename osforest<T, Allocator>::rep_node_type*
osforest<T, Allocator>::make_rep_node(rep_allocator_type& alloc, Args&&... args)
{
    std::cout << "++\n";
    auto             ptr = rep_allocator_traits::allocate(alloc, 1);
    scope_exit_guard on_exception(
        [&]() { rep_allocator_traits::deallocate(alloc, ptr, 1); });
    rep_allocator_traits::construct(alloc, ptr, std::forward<Args>(args)...);
    on_exception.dismiss();
    return ptr;
}

template <class T, class Allocator>
constexpr typename osforest<T, Allocator>::rep_node_type*
osforest<T, Allocator>::rep_node_deep_copy_node(rep_allocator_type&  alloc,
                                                rep_node_type const* n)
{
    if (n)
    {
        std::cout << "Copying: " << n->get() << "\n";
        auto ret = make_rep_node(alloc, n->get());
        assert(ret->next() == nullptr);
        assert(ret->prev() == nullptr);
        assert(ret->parent() == nullptr);
        assert(ret->first_child() == nullptr);
        assert(ret->last_child() == nullptr);

        if (n->first_child())
        {
            rep_node_deep_copy_from_node(alloc, n->first_child(), ret);
        }
        return ret;
    }
    return nullptr;
}

template <class T, class Allocator>
constexpr typename osforest<T, Allocator>::rep_node_type*
osforest<T, Allocator>::rep_node_deep_copy_from_node(
    rep_allocator_type& alloc, rep_node_type const* n,
    rep_node_type* copied_parent)
{
    if (n)
    {
        std::cout << "Copying: " << n->get() << "\n";
        auto ret = make_rep_node(alloc, n->get());
        assert(ret->next() == nullptr);
        assert(ret->prev() == nullptr);
        assert(ret->parent() == nullptr);
        assert(ret->first_child() == nullptr);
        assert(ret->last_child() == nullptr);

        if (n->first_child())
        {
            rep_node_deep_copy_from_node(alloc, n->first_child(), ret);
        }

        if (n->next())
        {
            ret->next_ =
                rep_node_deep_copy_from_node(alloc, n->next(), copied_parent);
        }

        if (copied_parent)
        {
            ret->parent_ = copied_parent;
            if (ret->prev() == nullptr)
            {
                copied_parent->first_child_ = ret;
            }
            if (ret->next() == nullptr)
            {
                copied_parent->last_child_ = ret;
            }
        }

        return ret;
    }
    return nullptr;
}

template <class T, class Allocator>
constexpr typename osforest<T, Allocator>::size_type
osforest<T, Allocator>::recursive_destroy_rep(rep_allocator_type& a,
                                              rep_node_type*      n)
{
    size_type ret = 0;
    if (n)
    {
        ++ret;
        std::cout << "--\n";
        ret += recursive_destroy_rep(a, n->first_child());
        ret += recursive_destroy_rep(a, n->next());
        rep_allocator_traits::destroy(a, n);
        rep_allocator_traits::deallocate(a, n, 1);
    }
    return ret;
}

template <class T, class Allocator>
osforest<T, Allocator>::osforest(osforest const& other)
{
    *this = other;
}

template <class T, class Allocator>
osforest<T, Allocator>::osforest(osforest&& other)
{
    *this = std::move(other);
}

template <class T, class Allocator>
osforest<T, Allocator>& osforest<T, Allocator>::operator=(osforest const& other)
{
    if (this != &other)
    {
        recursive_destroy_rep(alloc_, rep_first_root_);

        alloc_ = other.alloc_;

        rep_first_root_ = rep_node_deep_copy_from_node(
            alloc_, other.rep_first_root_, nullptr);

        for (rep_last_root_ = rep_first_root_;
             rep_last_root_ && rep_last_root_->next();
             rep_last_root_ = rep_last_root_->next())
        {
        }
    }
    return *this;
}

template <class T, class Allocator>
osforest<T, Allocator>& osforest<T, Allocator>::operator=(osforest&& other)
{
    if (this != &other)
    {
        alloc_          = std::move(other.alloc_);
        rep_first_root_ = std::exchange(other.rep_first_root_, nullptr);
        rep_last_root_  = std::exchange(other.rep_last_root_, nullptr);
    }
    return *this;
}

template <class T, class Allocator>
osforest<T, Allocator>::~osforest()
{
    assert((rep_first_root_ && rep_last_root_) ||
           (!rep_first_root_ && !rep_last_root_));

    recursive_destroy_rep(alloc_, rep_first_root_);
}

} // namespace sysml
