// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include <catch2/catch.hpp>

#include <iostream>
#include <string>

#include "sysml/osforest.hpp"

TEST_CASE("osforest", "[single-file]")
{
    int x = 3;
    CHECK(x == 3);

    {
        sysml::detail::osforest::rep_node<std::string> rn;
        CHECK(rn.get() == "");
    }

    {
        sysml::detail::osforest::rep_node<std::string> rn("zi");
        CHECK(rn.get() == "zi");
    }

    {
        sysml::osforest<std::string> osf(std::allocator<std::string>{});
        CHECK(osf.size() == 0);
    }

    {
        using sysml::detail::osforest::rep_node;
        using navigator =
            sysml::detail::osforest::basic_navigator<rep_node<int>, true>;
        using const_navigator =
            sysml::detail::osforest::basic_navigator<rep_node<int>, false>;

        rep_node<int> x1 = 3;
        rep_node<int> x2 = 3;

        navigator       n1(&x1);
        navigator       n2(&x2);
        navigator       n3(&x1);
        const_navigator cn1(&x1);
        const_navigator cn2(&x2);
        const_navigator cn3(&x1);

        CHECK(n1 != n2);
        CHECK(n1 == n3);

        CHECK(cn1 != cn2);
        CHECK(cn1 == cn3);

        n2 = cn1;
        CHECK(n1 == n2);
    }

    {
        sysml::osforest<std::string> rn;
        rn.roots_push_back("1");
        rn.roots_push_back("2");
        rn.roots_push_back("3");

        auto it = rn.roots_begin();
        CHECK(*it == "1");
        CHECK(*(it.next()) == "2");
        CHECK(*(it.next().next()) == "3");

        // {
        //     auto it = rn.roots_rbegin();
        //     CHECK(*it == "3");
        //     CHECK(*(it.prev()) == "2");
        //     CHECK(*(it.prev().prev()) == "1");
        // }

        {
            auto it = rn.roots_begin();
            it.move_to_next();
            auto r = rn.insert_after(it, "z");
            CHECK(*r == "z");
            CHECK(*(r.prev()) == "2");
        }

        {
            auto it = rn.roots_begin();
            CHECK(*it == "1");
            CHECK(*(it.next()) == "2");
            CHECK(*(it.next().next()) == "z");
            CHECK(*(it.next().next().next()) == "3");
        }

        {
            sysml::osforest<std::string> rn2;
            rn2 = rn;

            {
                auto it = rn2.roots_begin();
                CHECK(*it == "1");
                CHECK(*(it.next()) == "2");
                CHECK(*(it.next().next()) == "z");
                CHECK(*(it.next().next().next()) == "3");
            }
        }
    }
}

TEST_CASE("osforest-recursion", "[single-file]")
{
    sysml::osforest<std::string> forest;
    forest.roots_push_back("a");
    forest.roots_push_back("b");
    forest.roots_push_back("c");

    {
        auto nav = forest.roots_begin();
        {
            auto r = forest.children_push_front(nav, "a1");
            CHECK(!r.prev());
        }
        forest.children_emplace_front(nav, "a0");
        auto l = forest.children_emplace_back(nav, "a2");
        CHECK(!l.next());
        auto x = forest.insert_after(l, "a4");
        forest.insert_before(x, "a3");
    }

    {
        auto nav = forest.roots_begin();
        CHECK(*nav == "a");
        nav.move_to_first_child();
        CHECK(*nav == "a0");
        nav.move_to_next();
        CHECK(*nav == "a1");
        auto n2 = nav.next();
        CHECK(*n2 == "a2");
        CHECK(*(n2.next()) == "a3");
        CHECK(*(n2.next().next()) == "a4");
        CHECK(!(n2.next().next().next()));
        CHECK(n2.parent() == forest.roots_begin());
    }

    {
        auto x = forest.roots_emplace_before(forest.roots_end(), "d");
        CHECK(!x.next());
        CHECK(*(x.prev()) == "c");
    }

    {
        auto sf = forest.subforest(forest.roots_begin());
        auto x  = sf.roots_begin();
        CHECK(*x == "a");
        CHECK(!x.prev());
        CHECK(!x.next());
        x.move_to_first_child();
        CHECK(*x == "a0");
        CHECK(*(x.next()) == "a1");
        x.move_to_next().move_to_next();
        CHECK(*x == "a2");
        CHECK(*(x.next()) == "a3");
        x.move_to_next().move_to_next();
        CHECK(*x == "a4");
        CHECK(!x.next());
    }

    {
        auto sf = forest.subforest(forest.roots_begin(),
                                   forest.roots_begin().next().next());
        auto x  = sf.roots_begin();
        CHECK(*x == "a");
        CHECK(!x.prev());
        CHECK(x.next());
        CHECK(!x.next().next());
    }
}
