// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include <catch2/catch.hpp>

#include "sysml/smallvec.hpp"
#include "sysml/utility.hpp"

#include <iostream>
#include <string>

TEST_CASE("smallvec", "basic")
{
    int x = 3;
    {
        auto ose = sysml::scope_exit_guard([&]() { x = 0; });
    }
    CHECK(x == 0);

    {
        sysml::smallvec<int, 10> v;
        CHECK(v.size() == 0);
        v.push_back(3);
        CHECK(v.size() == 1);
        CHECK(v.at(0) == 3);
        CHECK(v[0] == 3);
        {
            auto const& vc = v;
            CHECK(vc.size() == 1);
            CHECK(vc.at(0) == 3);
            CHECK(vc[0] == 3);
        }

        v.push_back(4);
        v.push_back(5);

        CHECK(v.size() == 3);
        v.pop_back();
        CHECK(v.size() == 2);

        v.resize(10);
        CHECK(v.size() == 10);
    }
}

TEST_CASE("smallvec_construct_destruct", "mock")
{
    static int constructor_calls      = 0;
    static int copy_constructor_calls = 0;
    static int move_constructor_calls = 0;
    static int destructor_calls       = 0;

    struct element_type
    {
        element_type() noexcept { ++constructor_calls; }
        element_type(element_type const&) noexcept
        {
            ++copy_constructor_calls;
            ++constructor_calls;
        }
        element_type(element_type&&) noexcept
        {
            ++move_constructor_calls;
            ++constructor_calls;
        }
        ~element_type() { ++destructor_calls; }

        static void reset_counters()
        {
            constructor_calls      = 0;
            copy_constructor_calls = 0;
            move_constructor_calls = 0;
            destructor_calls       = 0;
        }
    };

    struct throwing_element_type
    {
        throwing_element_type() noexcept(false) { ++constructor_calls; }
        throwing_element_type(throwing_element_type const&) noexcept(false)
        {
            ++copy_constructor_calls;
            ++constructor_calls;
        }
        throwing_element_type(throwing_element_type&&) noexcept(false)
        {
            ++move_constructor_calls;
            ++constructor_calls;
        }
        ~throwing_element_type() { ++destructor_calls; }

        static void reset_counters()
        {
            constructor_calls      = 0;
            copy_constructor_calls = 0;
            move_constructor_calls = 0;
            destructor_calls       = 0;
        }
    };

    {

        sysml::smallvec<element_type, 20> vec(20);

        CHECK(vec.size() == 20);
        CHECK(constructor_calls == 20);
        vec.resize(5);
        CHECK(constructor_calls == 20);
        CHECK(destructor_calls == 15);

        // push_back
        {
            element_type e;
            vec.push_back(e);
            CHECK(constructor_calls == 22);
            CHECK(copy_constructor_calls == 1);
            vec.push_back(std::move(e));
            CHECK(constructor_calls == 23);
            CHECK(move_constructor_calls == 1);
        }

        CHECK(destructor_calls == 16);

        vec.emplace_back();
        CHECK(constructor_calls == 24);

        CHECK(destructor_calls == 16);
    }

    CHECK(constructor_calls == destructor_calls);
    element_type::reset_counters();
    {
        sysml::smallvec<element_type, 10> first(10);
        {
            sysml::smallvec<element_type, 10> second(10);
            first = second;
        }
        CHECK(constructor_calls == 30);
        CHECK(destructor_calls == 20);
        CHECK(copy_constructor_calls == 10);

        {
            sysml::smallvec<element_type, 10> third(5);
            CHECK(constructor_calls == 35);
            first = std::move(third);
            CHECK(destructor_calls == 35);
            CHECK(constructor_calls == 40);
            CHECK(move_constructor_calls == 5);
            CHECK(first.size() == 5);
            CHECK(first.max_size() == 10);
        }
    }

    CHECK(constructor_calls == destructor_calls);

    element_type::reset_counters();
    {
        sysml::smallvec<element_type, 12> a(10);
        sysml::smallvec<element_type, 12> b(5);
        std::swap(a, b);
        CHECK(a.size() == 5);
        CHECK(b.size() == 10);
    }
    CHECK(constructor_calls == destructor_calls);

    CHECK(std::is_nothrow_swappable_v<sysml::smallvec<element_type, 4>>);
    CHECK(!std::is_nothrow_swappable_v<
          sysml::smallvec<throwing_element_type, 4>>);

    {
        sysml::smallvec<int, 20> v(10);
        v.assign(3, 3);
        CHECK(v.size() == 3);
        for (auto s = v.size(); s; --s)
        {
            CHECK(v[s - 1] == 3);
        }

        v.assign(5, 5);
        CHECK(v.size() == 5);
        for (auto s = v.size(); s; --s)
        {
            CHECK(v[s - 1] == 5);
        }

        std::vector<int> svec{0, 1, 2, 3, 4, 5, 6};
        v.assign(svec.begin(), svec.end());
        CHECK(v.size() == 7);
        for (auto s = v.size(); s; --s)
        {
            CHECK(v[s - 1] == (int)(s - 1));
        }

        v.assign({1, 2, 3, 4, 5, 6});
        CHECK(v.size() == 6);
        for (auto s = v.size(); s; --s)
        {
            CHECK(v[s - 1] == (int)(s));
        }


    }

    //    sysml::smallvec<element_type, 20> other;
}
