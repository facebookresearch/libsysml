// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include <catch2/catch.hpp>

#include "sysml/ndarray.hpp"
#include "sysml/ndloop.hpp"
#include "sysml/numeric.hpp"
#include "sysml/vek.hpp"

#include <sstream>
#include <tuple>
#include <type_traits>

TEST_CASE("ndloop-0", "[single-file]")
{
    int me[2][3][4];

    using namespace sysml;

    vek<int, 3> range{2, 3, 4};
    ndloop(range, [&](auto const& v) { me[v[0]][v[1]][v[2]] = 0xdeadbeef; });

    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 3; ++j)
            for (int k = 0; k < 4; ++k)
                CHECK(me[i][j][k] == 0xdeadbeef);

    vek<int, 3> from{1, 1, 1};
    ndloop(from, range, [&](auto const& v) { ++me[v[0]][v[1]][v[2]]; });

    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 3; ++j)
            for (int k = 0; k < 4; ++k)
            {
                if (i == 0 || j == 0 || k == 0)
                {
                    CHECK(me[i][j][k] == 0xdeadbeef);
                }
                else
                {
                    CHECK(me[i][j][k] == 0xdeadbeef + 1);
                }
            }

    ndloop(range, [&](auto a, auto b, auto c) { me[a][b][c] = 0x12341234; });

    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 3; ++j)
            for (int k = 0; k < 4; ++k)
                CHECK(me[i][j][k] == 0x12341234);

    ndloop(from, range, [&](auto i, auto j, auto k) { ++me[i][j][k]; });

    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 3; ++j)
            for (int k = 0; k < 4; ++k)
            {
                if (i == 0 || j == 0 || k == 0)
                {
                    CHECK(me[i][j][k] == 0x12341234);
                }
                else
                {
                    CHECK(me[i][j][k] == 0x12341234 + 1);
                }
            }
}

TEST_CASE("vek-1", "[single-file]")
{
    CHECK(sysml::is_arithmetic_v<int>);
    CHECK(sysml::is_arithmetic_v<sysml::ivec<1, true, 2>>);
    CHECK(!sysml::is_arithmetic_v<std::string>);

    unsigned z = 0;
    unsigned w = -z;
    CHECK(w == -z);

    {
        using sysml::vek;
        vek<int, 3> a{1, 2, 3};

        {
            vek<int, 3>        a{1, 2, 3};
            std::array<int, 3> x{1, 1, 1};
            auto               b = a;
            a += x;
            CHECK(a == b + x);
        }

        {
            std::ostringstream oss;
            oss << a;
            CHECK(oss.str() == "1, 2, 3");
        }

        {
            std::ostringstream oss;
            oss << formatted(a, "", ", ", "");
            CHECK(oss.str() == "1, 2, 3");
        }

        {
            std::ostringstream oss;
            auto               b = ++a;
            b++;
            oss << formatted(std::move(b), "[", ";", "]");
            CHECK(oss.str() == "[3;4;5]");
        }

        {
            std::stringstream iss("1, 2, 3");
            vek<int, 3>       gold{1, 2, 3};
            vek<int, 3>       a;
            CHECK(iss >> a);
            CHECK(a == gold);
        }
        {
            std::stringstream iss("<1, 2, 3>");
            vek<int, 3>       gold{1, 2, 3};
            vek<int, 3>       a;
            CHECK(iss >> formatted(a, "<", ",", ">"));
            CHECK(a == gold);
        }
    }

    {
        using namespace sysml;

        vek<int, 3> a{1, 2, 3};
        vek<int, 3> b{2, 3, 4};

        CHECK(++a == b);
        CHECK(a++ == b);
        CHECK(--a == b);
        CHECK(a-- == b);
    }

    {
        using namespace sysml;

        vek<int, 3> a{1, 2, 3};
        float       b = 3;

        auto c = a + b;
        CHECK(c[0] == 4);

        auto d = b + a;
        CHECK(d[0] == 4);

        auto e = a + vek_cast<long long>(a);
        CHECK(e[0] == 2);
    }
}


TEST_CASE("to_vek", "[single-file]")
{
    int                a[2] = {1, 2};
    sysml::vek<int, 2> gold = {1, 2};

    CHECK(sysml::to_vek(a) == gold);
    CHECK(sysml::to_vek(std::move(a)) == gold);

    {
        std::array<int, 4> a{1, 2, 3, 4};
        auto               x = sysml::to_vek(a);
        auto               y = sysml::to_vek(std::move(a));
        CHECK(x == y);
    }

    auto b = sysml::vek_cast<long long>(gold);

    {
        auto goldc = gold;
        auto c     = sysml::vek_cast<long long>(goldc);
        CHECK(c == gold);
    }

    CHECK(b[0] == 1);
    CHECK(b[1] == 2);

    b += 1;
    CHECK(b[1] == 3);

    b += gold;

    CHECK(b[1] == 5);

    b &= 1;

    CHECK(b[1] == 1);

    b <<= 1;

    CHECK(b[1] == 2);

    b ^= 3;
    CHECK(b[1] == 1);

    b <<= 2;
    CHECK(b[1] == 4);

    b >>= 1;
    CHECK(b[1] == 2);

    b <<= gold;
    CHECK(b[1] == 8);

    CHECK(std::is_same_v<int, decltype(std::declval<int const&>() +
                                       std::declval<int const&>())>);
}

TEST_CASE("vek_random", "[single-file]")
{
    {
        sysml::vek<int, 2> a  = {1, 2};
        auto const&        ac = a;

        auto       b  = a;
        auto const cc = b;

        CHECK(*a.data() == 1);
        CHECK(*ac.data() == 1);

        CHECK(*(a.data() + 1) == 2);
        CHECK(*(ac.data() + 1) == 2);

        CHECK(ac.data() == a.cdata());

        CHECK(a.data() != b.data());
        CHECK(b.data() != cc.data());

        CHECK(a.begin() == a.data());
        CHECK(a.cbegin() == a.data());
        CHECK(ac.begin() == a.data());
        CHECK(ac.cbegin() == a.data());

        CHECK(a.end() == a.data() + 2);
        CHECK(a.cend() == a.data() + 2);
        CHECK(ac.end() == a.data() + 2);
        CHECK(ac.cend() == a.data() + 2);

        CHECK(a.rbegin() + 2 == a.rend());
        CHECK(a.crbegin() + 2 == a.crend());
        CHECK(ac.rbegin() + 2 == ac.rend());

        CHECK(a.size() == 2);
        CHECK(a.max_size() == 2);
        CHECK(!a.empty());

        CHECK(&(a[0]) == a.begin());
        CHECK(&(a.at(0)) == a.begin());

        CHECK(&(ac[0]) == ac.begin());
        CHECK(&(ac.at(0)) == ac.cbegin());

        CHECK(a[0] == 1);
        CHECK(a[1] == 2);

        CHECK(ac[0] == 1);
        CHECK(ac[1] == 2);

        b.fill(3);

        CHECK(b[0] == 3);
        CHECK(b[1] == 3);

        b.swap(a);

        CHECK(b[0] == 1);
        CHECK(b[1] == 2);

        CHECK(a[0] == 3);
        CHECK(a[1] == 3);
    }
}
