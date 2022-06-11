// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/numeric.hpp"

namespace sysml
{

template <arithmetic, unsigned>
class ndarray;

template <arithmetic, unsigned>
class ndarray_ref;

template <arithmetic, unsigned>
class const_ndarray_ref;

template <arithmetic, unsigned>
class ndarray_view;

template <arithmetic Tp, unsigned, class = Tp const*>
class const_ndarray_view;

template <arithmetic, unsigned>
class ndarray_view_proxy;

template <arithmetic Tp, unsigned, class = Tp const*>
class const_ndarray_view_proxy;

} // namespace sysml
