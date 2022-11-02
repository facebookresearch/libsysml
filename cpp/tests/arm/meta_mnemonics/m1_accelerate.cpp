// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#if defined(META_MNEMONICS_TEST_LIVE_INCLUDE)

#    include <catch2/catch.hpp>

#    include "../../utilities/random_vector.hpp"

#    include "sysml/code_generator.hpp"
#    include "sysml/code_generator/meta_mnemonics.hpp"
#    include "sysml/ndarray.hpp"

#    include <algorithm>
#    include <array>
#    include <cstddef>
#    include <cstdint>
#    include <numeric>
#    include <vector>

template <class Float>
inline void baseline_mm(sysml::ndarray_ref<Float, 2> const& A,
                        sysml::ndarray_ref<Float, 2> const& B,
                        sysml::ndarray_ref<Float, 2>& C, unsigned M, unsigned N,
                        unsigned K, int accumulate = 0)
{
    if (accumulate == 0)
    {
        for (unsigned m = 0; m < M; ++m)
        {
            for (unsigned n = 0; n < N; ++n)
            {
                C[m][n] = 0;
            }
        }
    }

    for (unsigned m = 0; m < M; ++m)
    {
        for (unsigned n = 0; n < N; ++n)
        {
            for (unsigned k = 0; k < K; ++k)
            {
                C[m][n] += A[m][k] * B[k][n];
                // std::cout << "C[m][n] = " << C[m][n]
                //           << ", A[m][k] = " << A[m][k]
                //           << ", B[k][n] = " << B[k][n] << ", (m, n, k) = ("
                //           << m
                //           << ", " << n << ", " << k << ")\n";
            }
        }
    }

    // for (unsigned i = 0; i < M * N; ++i)
    //     C.data()[i] = i;
}

TEST_CASE("meta_mnemonics::m1_accelerate", "[mm]")
{
    class mm : public sysml::code_generator::code_generator<int(
                   float const*, float const*, float*)>,
               public sysml::code_generator::meta_mnemonics::large_imm<mm>,
               public sysml::code_generator::meta_mnemonics::m1_accelerate<mm>

    {
    private:
        using large_imm = sysml::code_generator::meta_mnemonics::large_imm<mm>;
        // using large_imm =
        // sysml::code_generator::meta_mnemonics::large_imm<mm>;

    public:
        mm()
            : large_imm(x7)
        {
            enable_amx();

            meta_mov_imm(x9, 0x4000000000000000);
            // meta_mov_imm(x10, 0x4000000000000080);
            meta_mov_imm(x11, 1 << 27);
            meta_mov_imm(x12, 0x110000 ^ (1 << 27));
            meta_mov_imm(x13, 0x200040 ^ (1 << 27));
            meta_mov_imm(x14, 0x310040 ^ (1 << 27));
            meta_mov_imm(x15, 0x4LL << 56 | 0x80);
            meta_mov_imm(x10, 0x2LL << 56 | 16 * 0x80);
            orr(x0, x0, x9);
            orr(x1, x1, x9);
            orr(x2, x2, x9);
            mov(x3, x2);

            // Do this many times

            for (auto k = 0; k < 16; ++k)
            {
                // Load A[0:2]
                amxldx(x0);
                // Load B[0:2]
                amxldy(x1);
                // A[0] \otimes B[0]
                amxfma32(x11);
                // A[1] \otimes B[0]
                amxfma32(x12);
                // A[0] \otimes B[1]
                amxfma32(x13);
                // A[1] \otimes B[1]
                amxfma32(x14);

                if (k == 0)
                {
                    eor(x14, x14, x11);
                    eor(x13, x13, x11);
                    eor(x12, x12, x11);
                    eor(x11, x11, x11);
                }

                add(x0, x0, 0x80);
                add(x1, x1, 0x80);
            }

            for (int i = 0; i < 32; i++)
            {
                if (i % 2 == 1)
                {
                    continue;
                }
                amxstz(x2);
                add(x2, x2, x15);
            }

            add(x3, x3, x10);
            for (int i = 0; i < 32; i++)
            {
                if (i % 2 == 0)
                {
                    continue;
                }
                amxstz(x3);
                add(x3, x3, x15);
            }

            disable_amx();
            ret();
        }
    };

    using namespace sysml::test_utilities;

    using the_float = float;

    unsigned M = 32;
    unsigned N = 32;
    unsigned K = 16;

    auto inpA = aligned_vector<the_float, 128 * 2>(M * K);
    auto inpB = aligned_vector<the_float, 128 * 2>(N * K);
    auto outC = aligned_vector<the_float, 128 * 2>(N * M, 0);
    auto refC = aligned_vector<the_float, 128 * 2>(N * M, 132);

    sysml::ndarray_ref<the_float, 2> A(inpA.data(), {M, K},
                                       sysml::column_major_order);
    sysml::ndarray_ref<the_float, 2> B(inpB.data(), {K, N},
                                       sysml::row_major_order);
    sysml::ndarray_ref<the_float, 2> C_m1(outC.data(), {M, N},
                                          sysml::column_major_order);
    sysml::ndarray_ref<the_float, 2> C_ref(refC.data(), {M, N},
                                           sysml::column_major_order);

    for (int k = 0; k < K; ++k)
    {
        for (unsigned m_and_n = 0; m_and_n < M; ++m_and_n)
        {
            B[k][m_and_n] = (float)(rand() % 13) / 1;
            A[m_and_n][k] = (float)(rand() % 13) / 1;
        }
    }

    auto fn = mm().get_unique();

    baseline_mm(A, B, C_ref, M, N, K);

    fn(A.data(), B.data(), C_m1.data());

    for (unsigned m = 0; m < M; ++m)
    {
        for (unsigned n = 0; n < N; ++n)
        {
            CHECK(std::abs(C_m1[m][n] - C_ref[m][n]) < 0.0001);
        }
    }
}

#endif
