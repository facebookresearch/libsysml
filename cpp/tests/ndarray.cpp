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

TEST_CASE("ndarray", "[single-file]")
{
    {
        sysml::ndarray<int, 3> zi;
        CHECK(zi.data() == nullptr);
    }


    {
        sysml::ndarray<int, 2> z1;
        sysml::ndarray<int, 2> z2({2u, 2u});
        CHECK(z1.data() == nullptr);
        CHECK(z2.data() != nullptr);

        z1 = std::move(z2);

        CHECK(z1.data() != nullptr);
        CHECK(z2.data() == nullptr);

        {
            sysml::ndarray<int, 2> z3 = std::move(z1);
            CHECK(z1.data() == nullptr);
            CHECK(z3.data() != nullptr);
        }
        // sysml::ndarray<int, 3> z2 = std::move(zi);

        {
            std::vector<int>           v{0, 1, 2, 3, 4, 5, 6, 7};
            std::array<std::size_t, 3> shape{2, 2, 2};
            sysml::ndarray_ref<int, 3> nda(v.data(), shape);

            sysml::ndarray<int, 2> cp({2u, 2u});

            cp = nda[1];

            CHECK(cp[0][0] == 4);
            CHECK(cp[0][1] == 5);
            CHECK(cp[1][0] == 6);
            CHECK(cp[1][1] == 7);

            {
                sysml::ndarray_ref<int, 2> r2(v.data(), {2u, 2u});
                r2 = cp;

                CHECK(r2[0][0] == 4);
                CHECK(r2[0][1] == 5);
                CHECK(r2[1][0] == 6);
                CHECK(r2[1][1] == 7);
            }
        }
    }
}

TEST_CASE("ndarray-descriptor", "[single-file]")
{
    using namespace sysml::ndarray_detail;

    {
        owning_ndarray_profile<2> onda2{{2u, 2u}, {1u, 2u}};
        CHECK(onda2.extents_data()[0] == 2);

        nonowning_ndarray_profile<2> nonda2{onda2.extents_data(),
                                            onda2.strides_data()};
        CHECK(nonda2.strides_data()[0] == 1);
        CHECK(nonda2.strides_data()[1] == 2);
    }

    {
        owning_ndarray_profile<2> onda2{{2u, 2u}, {1u, 2u}};
        CHECK(onda2.native_extents_data()[0] == 2);

        nonowning_ndarray_profile<2> nonda2{onda2.extents_data(),
                                            onda2.strides_data()};
        CHECK(nonda2.native_strides_data()[0] == 1);
        CHECK(nonda2.native_strides_data()[1] == 2);
    }

    {
        ndarray_profile<2, ownership_kind::owning> onda2{{2u, 2u}, {1u, 2u}};
        CHECK(onda2.extents_data()[0] == 2);

        ndarray_profile<2, ownership_kind::nonowning> nonda2{
            onda2.extents_data(), onda2.strides_data()};
        CHECK(nonda2.strides_data()[0] == 1);
        CHECK(nonda2.strides_data()[1] == 2);
    }

    {
        ndarray_profile<2, ownership_kind::owning> onda2{{2u, 2u}, {1u, 2u}};
        CHECK(onda2.native_extents_data()[0] == 2);

        ndarray_profile<2, ownership_kind::nonowning> nonda2{
            onda2.extents_data(), onda2.strides_data()};
        CHECK(nonda2.native_strides_data()[0] == 1);
        CHECK(nonda2.native_strides_data()[1] == 2);
    }

    {
        ndarray_profile<3, ownership_kind::owning> op1{{2u, 3u, 4u},
                                                       {12u, 4u, 1u}};

        {
            auto sp1 = get_subprofile<ownership_kind::owning>(op1);
            CHECK(sp1.native_extents_data() ==
                  std::array<std::size_t, 2>{3u, 4u});
            CHECK(get_subprofile<ownership_kind::owning>(op1)
                      .native_strides_data() ==
                  std::array<std::size_t, 2>{4u, 1u});
        }

        {
            auto x1 = get_subprofile<ownership_kind::nonowning>(op1);
            auto x2 = get_subprofile<ownership_kind::owning>(x1);

            CHECK(x2.extents_data()[0] == 4);
            CHECK(x2.strides_data()[0] == 1);

            CHECK(x1.extents_data()[0] == 3);
            CHECK(x1.extents_data()[1] == 4);
            CHECK(x1.strides_data()[0] == 4);
            CHECK(x1.strides_data()[1] == 1);
        }
    }
}

TEST_CASE("cosnt_ndarray_ref-0", "[single-file]")
{
    std::vector<int>           v{0, 1, 2, 3, 4, 5, 6, 7, 8};
    std::array<std::size_t, 2> shape{3, 3};

    sysml::const_ndarray_ref<int, 2> nda(v.data(), shape);

    CHECK(nda.strides()[0] == 3);
    CHECK(nda.strides()[1] == 1);

    for (int i = 0, idx = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            CHECK(nda[i][j] == idx++);
}

TEST_CASE("ndarray_ref-0", "[single-file]")
{
    std::vector<int>           v{0, 1, 2, 3, 4, 5, 6, 7, 8};
    std::array<std::size_t, 2> shape{3, 3};

    sysml::ndarray_ref<int, 2> nda(v.data(), shape);

    CHECK(nda.strides()[0] == 3);
    CHECK(nda.strides()[1] == 1);

    for (int i = 0, idx = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
        {
            CHECK(nda[i][j] == idx++);
            ++nda[i][j];
        }

    for (int i = 0, idx = 1; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
        {
            CHECK(nda[i][j] == idx++);
            // ++nda[i][j];
        }
}

TEST_CASE("ndarray_ref-1", "[single-file]")
{
    std::vector<int>           v{0, 1, 2, 3, 4, 5, 6, 7, 8};
    std::array<std::size_t, 2> shape{3, 3};

    sysml::ndarray_ref<int, 2>       nda(v.data(), shape);
    sysml::const_ndarray_ref<int, 2> cnda(v.data(), shape);

    for (int i = 0; i < 3; ++i)
        CHECK(nda[0][i] != nda[1][i]);

    nda[0] = cnda[1];

    for (int i = 0; i < 3; ++i)
        CHECK(nda[0][i] == nda[1][i]);
}

TEST_CASE("ndarray_ref-2", "[single-file]")
{
    std::vector<int> v0{0, 1, 2, 3, 4, 5, 6, 7};
    std::vector<int> v1{0, 1, 2, 3, 4, 5, 6, 7};

    std::array<std::size_t, 3> shape{2, 2, 2};

    sysml::ndarray_ref<int, 3>       a1(v0.data(), shape);
    sysml::const_ndarray_ref<int, 3> ca2(v1.data(), shape);

    a1[1][1] = ca2[0][0];
    CHECK(a1[1][1][0] == 0);
    CHECK(a1[1][1][1] == 1);
    CHECK(ca2[1][1][0] == 6);
    CHECK(ca2[1][1][1] == 7);
}

TEST_CASE("ndarray-1", "[single-file]")
{
    std::vector<int>           v{0, 1, 2, 3, 4, 5, 6, 7};
    std::array<std::size_t, 3> shape{2, 2, 2};

    sysml::const_ndarray_ref<int, 3> nda(v.data(), shape);

    CHECK(nda.strides()[0] == 4);
    CHECK(nda.strides()[1] == 2);
    CHECK(nda.strides()[2] == 1);
    CHECK(nda[0].strides()[0] == 2);
    CHECK(nda[0].strides()[1] == 1);
    CHECK(nda[0][0].strides()[0] == 1);

    CHECK(nda[0][0][1] == 1);

    for (int i = 0, idx = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            for (int k = 0; k < 2; ++k, ++idx)
            {
                CHECK(nda[i][j].at(k) == idx);
                CHECK(nda[{i, j, k}] == idx);
                CHECK(nda.at({i, j, k}) == idx);
            }
}

TEST_CASE("ndarray-2", "[single-file]")
{
    std::vector<int>           v{0, 1, 2, 3, 4, 5, 6, 7};
    std::array<std::size_t, 3> shape{2, 2, 2};

    sysml::const_ndarray_ref<int, 3> nda(v.data(), shape,
                                         sysml::column_major_order);

    CHECK(nda.strides()[0] == 1);
    CHECK(nda.strides()[1] == 2);
    CHECK(nda.strides()[2] == 4);
    CHECK(nda[0].strides()[0] == 2);
    CHECK(nda[0].strides()[1] == 4);
    CHECK(nda[0][0].strides()[0] == 4);

    CHECK(nda[1][0][0] == 1);

    for (int i = 0, idx = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            for (int k = 0; k < 2; ++k, ++idx)
            {
                CHECK(nda[k][j].at(i) == idx);
                CHECK(nda[{k, j, i}] == idx);
                CHECK(nda.at({k, j, i}) == idx);
            }
}
