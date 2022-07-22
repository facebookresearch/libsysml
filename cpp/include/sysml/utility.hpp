// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include <stdexcept>
#include <utility>

// Checkout https://quuxplusone.github.io/blog/2019/02/06/arrow-proxy/

namespace sysml
{

template <class Reference>
struct arrow_proxy
{
    arrow_proxy(Reference const& r)
        : reference_(r)
    {
    }

    arrow_proxy(Reference&& r)
        : reference_(std::move(r))
    {
    }

    Reference* operator->() const { return &reference_; }

    mutable Reference reference_;
};

// Inspired by
// https://stackoverflow.com/questions/10270328/the-simplest-and-neatest-c11-scopeguard

enum struct scope_exit_policy
{
    always,
    no_exception,
    exception
};

template <class Fn>
class scope_exit_guard
{
public:
    scope_exit_guard(Fn&&              fn,
                     scope_exit_policy policy = scope_exit_policy::always)
        : fn_(fn)
        , policy_(policy)
        , uncaught_exceptions_on_construct_(std::uncaught_exceptions())
        , dismissed_(false)
    {
    }

    void dismiss() noexcept { dismissed_ = true; }

    ~scope_exit_guard()
    {
        if (policy_ == scope_exit_policy::always ||
            ((std::uncaught_exceptions() > uncaught_exceptions_on_construct_) &&
             (policy_ == scope_exit_policy::exception)))
        {
            if (!dismissed_)
            {
                try
                {
                    fn_(); // must not throw
                }
                catch (...)
                {
                    std::terminate();
                }
            }
        }
    }

private:
    Fn                fn_;
    scope_exit_policy policy_;
    int               uncaught_exceptions_on_construct_;
    bool              dismissed_;
};

} // namespace sysml
