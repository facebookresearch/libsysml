// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

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

} // namespace sysml
