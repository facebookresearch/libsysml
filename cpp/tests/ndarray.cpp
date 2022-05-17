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

TEST_CASE("ndarray-0", "[single-file]")
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
