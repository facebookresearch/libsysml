#include <catch2/catch.hpp>

#include "sysml/numeric.hpp"
#include "sysml/vek.hpp"

#include <tuple>
#include <type_traits>

TEST_CASE("vek-1", "[single-file]")
{
    CHECK(sysml::is_arithmetic_v<int>);
    CHECK(sysml::is_arithmetic_v<sysml::ivec<1, true, 2>>);
    CHECK(!sysml::is_arithmetic_v<std::string>);

    unsigned z = 0;
    unsigned w = -z;
    CHECK(w == -z);

    {
        using namespace sysml;

        vek<int, 3> a{1, 2, 3};
        float b = 3;

        auto c = a + b;
        CHECK(c[0] == 4);

        auto d = b + a;
        CHECK(d[0] == 4);

        auto e = a + vek_cast<long long>(a);
        CHECK(e[0] == 2);

        // float z = 3;
        // auto w = z << 2;
        // CHECK(w == 3);

        // CHECK(std::is_same_v<int, typename decltype(

        {
            auto b = -a;
            CHECK(b[0] == -1);
        }
    }
}

TEST_CASE("vek1", "[single-file]")
{
    CHECK(std::tuple_size_v<sysml::vek<int, 3>> == 3);
    CHECK(std::is_same_v<std::tuple_element_t<0, sysml::vek<int, 3>>, int>);

    CHECK(
        sysml::helpers::is_plus_equal_compound_assignable<int,
                                                          float const&>::value);
    CHECK(sysml::helpers::is_plus_equal_compound_assignable<int, long>::value);

    CHECK(sysml::helpers::is_minus_equal_compound_assignable<
          int, float const&>::value);
}

TEST_CASE("to_vek", "[single-file]")
{
    int                a[2] = {1, 2};
    sysml::vek<int, 2> gold = {1, 2};

    CHECK(sysml::to_vek(a) == gold);
    CHECK(sysml::to_vek(std::move(a)) == gold);

    {
        std::array<int, 4> a{1, 2, 3, 4};
        auto x = sysml::to_vek(a);
        auto y = sysml::to_vek(std::move(a));
        CHECK(x == y);
    }

    auto b = sysml::vek_cast<long long>(gold);

    {
        auto goldc = gold;
        auto c = sysml::vek_cast<long long>(goldc);
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

    CHECK(sysml::helpers::is_plus_operable_from<int, short>::value);
    CHECK(sysml::helpers::is_minus_operable_from<int, short>::value);
    CHECK(sysml::helpers::is_times_operable_from<int, short>::value);
    CHECK(sysml::helpers::is_div_operable_from<int, short>::value);
    CHECK(sysml::helpers::is_mod_operable_from<int, short>::value);

    CHECK(sysml::helpers::is_band_operable_from<int, short>::value);
    CHECK(sysml::helpers::is_bor_operable_from<int, short>::value);
    CHECK(sysml::helpers::is_bxor_operable_from<int, short>::value);
    CHECK(sysml::helpers::is_bshl_operable_from<int, short>::value);
    CHECK(sysml::helpers::is_bshr_operable_from<int, short>::value);

    CHECK(sysml::helpers::is_land_operable_from<int, short>::value);
    CHECK(sysml::helpers::is_lor_operable_from<int, short>::value);

    CHECK(std::is_same_v<sysml::helpers::result_of_plus_from_t<int, int>, int>);

    //     CHECK(b[0] == 3);
    //     CHECK(b[1] == 4);

    //     auto c = 2 + b;
    //     CHECK(c[0] == 5);
    //     CHECK(c[1] == 6);

    //     auto d = c + c;
    //     CHECK(d[0] == 10);
    //     CHECK(d[1] == 12);
    // }

    // {
    //     vek<int, 2> a = {1, 2};
    //     vek<int, 2> b = {3, 4};

    //     vek<vek<int, 2>, 2> aa{a, b};
    //     vek<vek<int, 2>, 2> bb{b, a};

    //     CHECK(aa[0] == bb[1]);
    //     CHECK(aa[1] == bb[0]);

    //     auto cc = aa + a;
    // }
}

TEST_CASE("ZZZZ", "[single-file]")
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
