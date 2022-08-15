// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include <catch2/catch.hpp>

#include "sysml/memory.hpp"

TEST_CASE("all_tests", "")
{
    int                      A = 3;
    int                      B = 4;
    sysml::observed_ptr<int> p1a(&A);
    sysml::observed_ptr<int> p1b(&B);
    sysml::observed_ptr<int> p2a(&A);
    sysml::observed_ptr<int> p2b(&B);
    sysml::observed_ptr<int> pnull;

    CHECK(&A == p1a.get());
    CHECK(&A == p2a.get());
    CHECK(nullptr == pnull);
    CHECK(pnull == nullptr);
    CHECK(nullptr != p1a);
    CHECK(p1a != nullptr);

    CHECK(p1a >= p2a);
    CHECK(p1a <= p2a);
    CHECK(!(p1a > p2a));
    CHECK(!(p1a < p2a));
    CHECK(p1a == p2a);
    CHECK(!(p1a != p2a));

    CHECK(((p1a >= p2a) || (p1a < p2a)));
    CHECK(((p1a <= p2a) || (p1a > p2a)));

    {
        CHECK(p1a != pnull);
        CHECK(pnull == pnull);
        CHECK(p1a != nullptr);
        CHECK(nullptr != p1a);
        CHECK(!!p1a);
        CHECK(!pnull);
    }

    {
        sysml::observed_ptr<int> z;
        z = p1a;
        CHECK(z == p1a);
        CHECK(z != p1b);
    }

    {
        sysml::observed_ptr<void> z;
        z = p1a;
        CHECK(z == p1a);
        CHECK(z != p1b);
    }
}
