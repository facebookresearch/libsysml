// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#if defined(META_MNEMONICS_TEST_LIVE_INCLUDE)

#    include <catch2/catch.hpp>

#    include "sysml/code_generator.hpp"
#    include "sysml/code_generator/meta_mnemonics.hpp"
#    include "sysml/vek.hpp"

#    include <algorithm>
#    include <cstdint>
#    include <numeric>
#    include <numeric> // for std::iota
#    include <vector>

namespace sysml::tests::meta_mnemonics
{

template <class T>
inline std::vector<T> get_iota_vector(std::size_t size)
{
    std::vector<T> ret(size);
    std::iota(std::begin(ret), std::end(ret), static_cast<T>(0));
    return ret;
}

template <class T>
void check_transposed_2d(T const* lhs, T const* rhs, int R, int C,
                         int lhs_R_stride, int lhs_C_stride, int rhs_R_stride,
                         int rhs_C_stride)
{
    for (int r = 0; r < R; ++r)
    {
        for (int c = 0; c < C; ++c)
        {
            CHECK(lhs[r * lhs_R_stride + c * lhs_C_stride] ==
                  rhs[r * rhs_R_stride + c * rhs_C_stride]);
        }
    }
}

template <class T>
void naive_transpose_2d(T* lhs, T const* rhs, int R, int C, int lhs_R_stride,
                        int lhs_C_stride, int rhs_R_stride, int rhs_C_stride)
{
    for (int r = 0; r < R; ++r)
    {
        for (int c = 0; c < C; ++c)
        {
            lhs[r * lhs_R_stride + c * lhs_C_stride] =
                rhs[r * rhs_R_stride + c * rhs_C_stride];
        }
    }
}

template <class T>
void naive_trn_pair_inplace(T* d, int len)
{
    SYSML_STRONG_ASSERT(len % 2 == 0);
    for (int l = 0; l < len; l += 2)
    {
        std::swap(d[l], d[l + len + 1]);
    }
}

template <class T>
void check_trn_pair(T const* lhs, T const* rhs, int len)
{
    SYSML_STRONG_ASSERT(len % 2 == 0);
    for (int l = 0; l < len; l += 2)
    {
        CHECK(lhs[l] == rhs[l]);
        CHECK(lhs[l + 1] == rhs[l + len]);
        CHECK(lhs[l + len] == rhs[l + 1]);
        CHECK(lhs[l + len + 1] == rhs[l + len + 1]);
    }
}

template <class T>
void check_trnN_pair(T const* lhs, T const* rhs, int N, int Vlen)
{
    SYSML_STRONG_ASSERT(Vlen % N == 0);
    for (int l = 0; l < Vlen; l += N)
    {
        for (int r = 0; r < N; ++r)
        {
            for (int c = 0; c < N; ++c)
            {
                CHECK(lhs[r * Vlen + c] == rhs[c * Vlen + r]);
            }
        }
        lhs += N;
        rhs += N;
    }
}

template <class T>
struct unmatched
{
    using type = T;
};

template <class T>
using unmatched_t = typename unmatched<T>::type;

using sysml::code_generator::meta_mnemonics::vreg_view;

class transposer_test_base : public sysml::code_generator::basic_code_generator
{
public:
    template <unsigned L, unsigned N>
    void load_all(vreg_view<L, N> const&                                 v,
                  std::vector<sysml::code_generator::xbyak::VReg> const& r,
                  sysml::code_generator::xbyak::XReg const&              addr)
    {
        for (unsigned i = 0; i + 1 < r.size(); i += 2)
        {
            ldp(v.to_simd_scalar(r[i]), v.to_simd_scalar(r[i + 1]),
                post_ptr(addr, L * N * 2));
        }
        if (r.size() % 2)
        {
            ldr(v.to_simd_scalar(r.back()), ptr(addr));
        }
        sub_imm(addr, addr, (r.size() / 2) * L * N * 2, x1);
    }

    template <unsigned L, unsigned N>
    void store_all(vreg_view<L, N> const&                                 v,
                   std::vector<sysml::code_generator::xbyak::VReg> const& r,
                   sysml::code_generator::xbyak::XReg const&              addr)
    {
        for (unsigned i = 0; i + 1 < r.size(); i += 2)
        {
            stp(v.to_simd_scalar(r[i]), v.to_simd_scalar(r[i + 1]),
                post_ptr(addr, L * N * 2));
        }
        if (r.size() % 2)
        {
            str(v.to_simd_scalar(r.back()), ptr(addr));
        }
        sub_imm(addr, addr, (r.size() / 2) * L * N * 2, x1);
    }
};

template <class T, unsigned N = 4>
class pair_transposer
    : public sysml::code_generator::basic_code_generator,
      public sysml::code_generator::with_signature<pair_transposer<T, N>,
                                                   void(T const*)>,
      public sysml::code_generator::meta_mnemonics::vreg_transpose<
          pair_transposer<T, N>>
{
private:
    using trn = sysml::code_generator::meta_mnemonics::vreg_transpose<
        pair_transposer<T, N>>;

public:
    explicit pair_transposer(unsigned algo)
    {
        sysml::code_generator::meta_mnemonics::vreg_view<sizeof(T), N> v;
        ldp(v.to_simd_scalar(v0), v.to_simd_scalar(v1), ptr(x0));

        switch (algo % 6)
        {
        case 0:
            trn::trn2s_inplace(v, v0, v1, v2);
            stp(v.to_simd_scalar(v0), v.to_simd_scalar(v1), ptr(x0));
            break;
        case 1:
            trn::trn2s_keep_first(v, v0, v1, v2);
            stp(v.to_simd_scalar(v0), v.to_simd_scalar(v2), ptr(x0));
            break;
        case 2:
            trn::trn2s_onto(v, v0, v1, v2, v3);
            stp(v.to_simd_scalar(v2), v.to_simd_scalar(v3), ptr(x0));
            break;
        case 3:
            trn::trn2s_inplace(v, std::tie(v0, v1), std::tie(v2));
            stp(v.to_simd_scalar(v0), v.to_simd_scalar(v1), ptr(x0));
            break;
        case 4:
            trn::trn2s_onto(v, std::tie(v0, v1), std::tie(v2, v3));
            stp(v.to_simd_scalar(v2), v.to_simd_scalar(v3), ptr(x0));
            break;
        default:
            trn::trn2s_keep_second(v, v0, v1, v2);
            stp(v.to_simd_scalar(v2), v.to_simd_scalar(v1), ptr(x0));
            break;
        }

        ret();
    }
};

template <class T, unsigned N = 4>
class trn4_transposer
    : public transposer_test_base,
      public sysml::code_generator::with_signature<trn4_transposer<T, N>,
                                                   void(T const*)>,
      public sysml::code_generator::meta_mnemonics::vreg_transpose<
          trn4_transposer<T, N>>
{
private:
    using trn = sysml::code_generator::meta_mnemonics::vreg_transpose<
        trn4_transposer<T, N>>;

public:
    explicit trn4_transposer(unsigned algo)
    {
        vreg_view<sizeof(T), N> v;
        load_all(v, {v0, v1, v2, v3}, x0);

        switch (algo % 2)
        {
        case 0:
            trn::trn4s_inplace(v, std::tie(v0, v1, v2, v3), std::tie(v4, v5));
            store_all(v, {v0, v1, v2, v3}, x0);
            break;
        default:
            trn::trn4s_onto(v, std::tie(v0, v1, v2, v3),
                            std::tie(v4, v5, v6, v7));
            store_all(v, {v4, v5, v6, v7}, x0);
            break;
        }

        ret();
    }
};

template <class T, unsigned N = 8>
class trn8_transposer
    : public transposer_test_base,
      public sysml::code_generator::with_signature<trn8_transposer<T, N>,
                                                   void(T const*)>,
      public sysml::code_generator::meta_mnemonics::vreg_transpose<
          trn8_transposer<T, N>>
{
private:
    using trn = sysml::code_generator::meta_mnemonics::vreg_transpose<
        trn8_transposer<T, N>>;

public:
    explicit trn8_transposer(unsigned algo)
    {
        sysml::code_generator::meta_mnemonics::vreg_view<sizeof(T), N> v;
        load_all(v, {v0, v1, v2, v3, v4, v5, v6, v7}, x0);

        switch (algo % 2)
        {
        case 0:
            trn::trn8s_inplace(v, std::tie(v0, v1, v2, v3, v4, v5, v6, v7),
                               std::tie(v8, v9));
            store_all(v, {v0, v1, v2, v3, v4, v5, v6, v7}, x0);
            break;
        default:
            trn::trn8s_onto(v, std::tie(v0, v1, v2, v3, v4, v5, v6, v7),
                            std::tie(v8, v9, v10, v11, v12, v13, v14, v15));
            store_all(v, {v8, v9, v10, v11, v12, v13, v14, v15}, x0);
            break;
        }

        ret();
    }
};

template <class T, unsigned N = 16>
class trn16_transposer
    : public transposer_test_base,
      public sysml::code_generator::with_signature<trn16_transposer<T, N>,
                                                   void(T const*)>,
      public sysml::code_generator::meta_mnemonics::vreg_transpose<
          trn16_transposer<T, N>>
{
private:
    using trn = sysml::code_generator::meta_mnemonics::vreg_transpose<
        trn16_transposer<T, N>>;

public:
    explicit trn16_transposer(unsigned algo)
    {
        sysml::code_generator::meta_mnemonics::vreg_view<sizeof(T), N> v;
        load_all(v,
                 {v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13,
                  v14, v15},
                 x0);

        switch (algo % 2)
        {
        case 0:
            trn::trn16s_inplace(v,
                                std::tie(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9,
                                         v10, v11, v12, v13, v14, v15),
                                std::tie(v16, v17));
            store_all(v,
                      {v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12,
                       v13, v14, v15},
                      x0);
            break;
        default:
            trn::trn16s_onto(v,
                             std::tie(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9,
                                      v10, v11, v12, v13, v14, v15),
                             std::tie(v16, v17, v18, v19, v20, v21, v22, v23,
                                      v24, v25, v26, v27, v28, v29, v30, v31));
            store_all(v,
                      {v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26,
                       v27, v28, v29, v30, v31},
                      x0);
            break;
        }

        ret();
    }
};

template <class T>
struct type_wrap
{
};

template <unsigned N>
struct uint_wrap
{
};

} // namespace sysml::tests::meta_mnemonics

TEST_CASE("meta_mnemonics::vreg_transpose2", "[trn2]")
{

    {
        using namespace sysml::tests::meta_mnemonics;

        auto const tester = []<class T, unsigned C>(type_wrap<T>, uint_wrap<C>)
        {
            auto gold = get_iota_vector<T>(C * 2);
            // naive_transpose_2d(gold.data(), in.data(), R, C, C, 1, 1, R);

            for (unsigned algo = 0; algo < 3; ++algo)
            {
                auto in = gold;
                auto fn = pair_transposer<T, C>(algo).get_unique();
                fn(in.data());
                check_trnN_pair(gold.data(), in.data(), 2, C);
            }
        };

        tester(type_wrap<std::uint8_t>{}, uint_wrap<4>{});
        tester(type_wrap<std::uint8_t>{}, uint_wrap<8>{});
        tester(type_wrap<std::uint8_t>{}, uint_wrap<16>{});

        tester(type_wrap<std::uint16_t>{}, uint_wrap<2>{});
        tester(type_wrap<std::uint16_t>{}, uint_wrap<4>{});
        tester(type_wrap<std::uint16_t>{}, uint_wrap<8>{});
        tester(type_wrap<float>{}, uint_wrap<2>{});
        tester(type_wrap<float>{}, uint_wrap<4>{});
        tester(type_wrap<double>{}, uint_wrap<2>{});
    }
}

TEST_CASE("meta_mnemonics::vreg_transpose4", "[trn4]")
{

    {
        using namespace sysml::tests::meta_mnemonics;

        auto const tester = []<class T, unsigned C>(type_wrap<T>, uint_wrap<C>)
        {
            auto gold = get_iota_vector<T>(C * 4);
            // naive_transpose_2d(gold.data(), in.data(), R, C, C, 1, 1, R);

            for (unsigned algo = 0; algo < 2; ++algo)
            {
                auto in = gold;
                auto fn = trn4_transposer<T, C>(algo).get_unique();
                fn(in.data());
                check_trnN_pair(gold.data(), in.data(), 4, C);
            }
        };

        tester(type_wrap<std::uint8_t>{}, uint_wrap<4>{});
        tester(type_wrap<std::uint8_t>{}, uint_wrap<8>{});
        tester(type_wrap<std::uint8_t>{}, uint_wrap<16>{});

        tester(type_wrap<std::uint16_t>{}, uint_wrap<4>{});
        tester(type_wrap<std::uint16_t>{}, uint_wrap<8>{});
        tester(type_wrap<float>{}, uint_wrap<4>{});
    }
}

TEST_CASE("meta_mnemonics::vreg_transpose8", "[trn8]")
{

    {
        using namespace sysml::tests::meta_mnemonics;

        auto const tester = []<class T, unsigned C>(type_wrap<T>, uint_wrap<C>)
        {
            auto gold = get_iota_vector<T>(C * 8);
            // naive_transpose_2d(gold.data(), in.data(), R, C, C, 1, 1, R);

            for (unsigned algo = 0; algo < 2; ++algo)
            {
                auto in = gold;
                auto fn = trn8_transposer<T, C>(algo).get_unique();
                fn(in.data());
                check_trnN_pair(gold.data(), in.data(), 8, C);
            }
        };

        tester(type_wrap<std::uint8_t>{}, uint_wrap<8>{});
        tester(type_wrap<std::uint8_t>{}, uint_wrap<16>{});

        tester(type_wrap<std::uint16_t>{}, uint_wrap<8>{});
    }
}

TEST_CASE("meta_mnemonics::vreg_transpose16", "[trn16]")
{

    {
        using namespace sysml::tests::meta_mnemonics;

        auto const tester = []<class T, unsigned C>(type_wrap<T>, uint_wrap<C>)
        {
            auto gold = get_iota_vector<T>(C * 16);
            // naive_transpose_2d(gold.data(), in.data(), R, C, C, 1, 1, R);

            for (unsigned algo = 0; algo < 2; ++algo)
            {
                auto in = gold;
                auto fn = trn16_transposer<T, C>(algo).get_unique();
                fn(in.data());
                check_trnN_pair(gold.data(), in.data(), 16, C);
            }
        };

        tester(type_wrap<std::uint8_t>{}, uint_wrap<16>{});
    }
}

#else
#    error "Can't include this file"
#endif
