// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/assert.hpp"
#include "sysml/code_generator/meta_mnemonics/arm/vreg_view.hpp"
#include "sysml/code_generator/xbyak.hpp"
#include "sysml/math.hpp"
#include "sysml/miltuple.hpp"

#include <array>
#include <vector>

namespace sysml::code_generator::meta_mnemonics
{

template <class CodeGenerator>
class vreg_transpose
{

private:
    CodeGenerator& self() { return static_cast<CodeGenerator&>(*this); }

public:
    template <unsigned ElementSize, unsigned NumElements>
    void trn2s_keep_first(vreg_view<ElementSize, NumElements> v,
                          xbyak::VReg const& r0, xbyak::VReg const& r1,
                          xbyak::VReg const& t)
    {
        static_assert(NumElements > 1 && sysml::is_even(NumElements));
        self().trn2(v[t], v[r0], v[r1]);
        self().trn1(v[r0], v[r0], v[r1]);
    }

    template <unsigned ElementSize, unsigned NumElements>
    void trn2s_keep_second(vreg_view<ElementSize, NumElements> v,
                           xbyak::VReg const& r0, xbyak::VReg const& r1,
                           xbyak::VReg const& t)
    {
        static_assert(NumElements > 1 && sysml::is_even(NumElements));
        self().trn1(v[t], v[r0], v[r1]);
        self().trn2(v[r1], v[r0], v[r1]);
    }

    template <unsigned ElementSize, unsigned NumElements>
    void trn2s_inplace(vreg_view<ElementSize, NumElements> v,
                       xbyak::VReg const& r0, xbyak::VReg const& r1,
                       xbyak::VReg const& t)
    {
        static_assert(NumElements > 1 && sysml::is_even(NumElements));
        self().mov(t.b16, r0.b16);
        self().trn1(v[r0], v[t], v[r1]);
        self().trn2(v[r1], v[t], v[r1]);
    }

    template <unsigned ElementSize, unsigned NumElements>
    void trn2s_onto(vreg_view<ElementSize, NumElements> v,
                    xbyak::VReg const& s0, xbyak::VReg const& s1,
                    xbyak::VReg const& d0, xbyak::VReg const& d1)
    {
        static_assert(NumElements > 1 && sysml::is_even(NumElements));
        self().trn1(v[d0], v[s0], v[s1]);
        self().trn2(v[d1], v[s0], v[s1]);
    }

    template <unsigned ElementSize, unsigned NumElements>
    void trn2s_inplace(vreg_view<ElementSize, NumElements>           v,
                       sysml::miltuple<xbyak::VReg const&, 2> const& r,
                       sysml::miltuple<xbyak::VReg const&, 1> const& tmp)
    {
        auto const& [r0, r1] = r;
        auto const& [t0]     = tmp;

        trn2s_inplace(v, r0, r1, t0);
    }

    template <unsigned ElementSize, unsigned NumElements>
    void trn2s_onto(vreg_view<ElementSize, NumElements>           v,
                    sysml::miltuple<xbyak::VReg const&, 2> const& src,
                    sysml::miltuple<xbyak::VReg const&, 2> const& dst)

    {
        auto const& [src0, src1] = src;
        auto const& [dst0, dst1] = dst;

        trn2s_onto(v, src0, src1, dst0, dst1);
    }

    template <unsigned ElementSize, unsigned NumElements>
    void trn2s_inplace(vreg_view<ElementSize, NumElements> v,
                       std::array<xbyak::VReg, 2> const&   r,
                       std::array<xbyak::VReg, 1> const&   t)
    {
        trn2s_inplace(v, r[0], r[1], t[0]);
    }

    template <unsigned ElementSize, unsigned NumElements>
    void trn2s_onto(vreg_view<ElementSize, NumElements> v,
                    std::array<xbyak::VReg, 2> const&   src,
                    std::array<xbyak::VReg, 2> const&   dst)

    {
        trn2s_onto(v, src[0], src[1], dst[0], dst[1]);
    }

    template <unsigned ElementSize, unsigned NumElements>
    void trn4s_inplace(vreg_view<ElementSize, NumElements>           v,
                       sysml::miltuple<xbyak::VReg const&, 4> const& r,
                       sysml::miltuple<xbyak::VReg const&, 2> const& tmp)
    {
        auto const& [r0, r1, r2, r3] = r;
        auto const& [t0, t1]         = tmp;

        trn2s_keep_first(v, r0, r1, t0);
        trn2s_keep_second(v, r2, r3, t1);

        vreg_view<ElementSize * 2, NumElements / 2> v2;

        trn2s_keep_first(v2, r0, t1, r2);
        trn2s_keep_second(v2, t0, r3, r1);
    }

    template <unsigned ElementSize, unsigned NumElements>
    void trn4s_onto(vreg_view<ElementSize, NumElements>           v,
                    sysml::miltuple<xbyak::VReg const&, 4> const& src,
                    sysml::miltuple<xbyak::VReg const&, 4> const& dst)

    {
        auto const& [src0, src1, src2, src3] = src;
        auto const& [dst0, dst1, dst2, dst3] = dst;

        trn2s_keep_first(v, src0, src1, dst1); //
        trn2s_keep_second(v, src2, src3, dst2);

        vreg_view<ElementSize * 2, NumElements / 2> v2;

        trn2s_keep_first(v2, dst1, src3, dst3);
        trn2s_keep_second(v2, src0, dst2, dst0);
    }

    template <unsigned ElementSize, unsigned NumElements>
    void trn8s_inplace(vreg_view<ElementSize, NumElements>           v,
                       sysml::miltuple<xbyak::VReg const&, 8> const& r,
                       sysml::miltuple<xbyak::VReg const&, 2> const& tmp)

    {
        auto const& [r0, r1, r2, r3, r4, r5, r6, r7] = r;
        auto const& [t0, t1]                         = tmp;

        vreg_view<ElementSize * 2, NumElements / 2> v2;
        vreg_view<ElementSize * 4, NumElements / 4> v4;

        trn4s_inplace(v, std::tie(r4, r5, r6, r7), tmp);

        trn2s_keep_first(v, r0, r1, t0);
        trn2s_keep_second(v, r2, r3, t1);

        trn2s_keep_second(v2, r0, t1, r1);
        trn2s_keep_second(v4, r1, r4, r0);
        trn2s_keep_second(v4, t1, r6, r2);

        trn2s_keep_first(v2, t0, r3, t1);
        trn2s_keep_second(v4, t0, r5, r1);
        trn2s_keep_second(v4, t1, r7, r3);
    }

    template <unsigned ElementSize, unsigned NumElements>
    void trn8s_onto(vreg_view<ElementSize, NumElements>           v,
                    sysml::miltuple<xbyak::VReg const&, 8> const& src,
                    sysml::miltuple<xbyak::VReg const&, 8> const& dst)

    {
        auto const& [src0, src1, src2, src3, src4, src5, src6, src7] = src;
        auto const& [dst0, dst1, dst2, dst3, dst4, dst5, dst6, dst7] = dst;

        vreg_view<ElementSize * 4, NumElements / 4> v4;

        trn4s_inplace(v, std::tie(src0, src1, src2, src3),
                      std::tie(dst0, dst2));
        trn4s_onto(v, std::tie(src4, src5, src6, src7),
                   std::tie(dst4, dst5, dst6, dst7));

        trn2s_keep_second(v4, src0, dst4, dst0);
        trn2s_keep_second(v4, src1, dst5, dst1);
        trn2s_keep_second(v4, src2, dst6, dst2);
        trn2s_keep_second(v4, src3, dst7, dst3);
    }

    template <unsigned ElementSize, unsigned NumElements>
    void trn16s_inplace(vreg_view<ElementSize, NumElements>            v,
                        sysml::miltuple<xbyak::VReg const&, 16> const& r,
                        sysml::miltuple<xbyak::VReg const&, 2> const&  tmp)

    {
        auto const& [r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13,
                     r14, r15] = r;

        vreg_view<ElementSize * 4, NumElements / 4> v4;

        trn4s_inplace(v, std::tie(r0, r1, r2, r3), tmp);
        trn4s_inplace(v, std::tie(r4, r5, r6, r7), tmp);
        trn4s_inplace(v, std::tie(r8, r9, r10, r11), tmp);
        trn4s_inplace(v, std::tie(r12, r13, r14, r15), tmp);

        trn4s_inplace(v4, std::tie(r0, r4, r8, r12), tmp);
        trn4s_inplace(v4, std::tie(r1, r5, r9, r13), tmp);
        trn4s_inplace(v4, std::tie(r2, r6, r10, r14), tmp);
        trn4s_inplace(v4, std::tie(r3, r7, r11, r15), tmp);
    }

    template <unsigned ElementSize, unsigned NumElements>
    void trn16s_onto(vreg_view<ElementSize, NumElements>            v,
                     sysml::miltuple<xbyak::VReg const&, 16> const& src,
                     sysml::miltuple<xbyak::VReg const&, 16> const& dst)

    {
        auto const& [s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13,
                     s14, s15] = src;
        auto const& [d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12, d13,
                     d14, d15] = dst;

        vreg_view<ElementSize * 4, NumElements / 4> v4;

        auto tmp = std::tie(d1, d2);

        trn4s_inplace(v, std::tie(s0, s1, s2, s3), tmp);
        trn4s_inplace(v, std::tie(s4, s5, s6, s7), tmp);
        trn4s_inplace(v, std::tie(s8, s9, s10, s11), tmp);
        trn4s_inplace(v, std::tie(s12, s13, s14, s15), tmp);

        trn4s_onto(v4, std::tie(s0, s4, s8, s12), std::tie(d0, d4, d8, d12));
        trn4s_onto(v4, std::tie(s1, s5, s9, s13), std::tie(d1, d5, d9, d13));
        trn4s_onto(v4, std::tie(s2, s6, s10, s14), std::tie(d2, d6, d10, d14));
        trn4s_onto(v4, std::tie(s3, s7, s11, s15), std::tie(d3, d7, d11, d15));
    }

    template <unsigned ElementSize, unsigned NumElements>
    void
    transpose_inplace(vreg_view<ElementSize, NumElements>                     v,
                      sysml::miltuple<xbyak::VReg const&, NumElements> const& r,
                      sysml::miltuple<xbyak::VReg const&, 2> const& tmp)
    {
        static_assert(NumElements == 2 || NumElements == 4 ||
                      NumElements == 8 || NumElements == 16);

        if constexpr (NumElements == 2)
        {
            trn2s_inplace(v, r, tmp);
        }
        else if constexpr (NumElements == 4)
        {
            trn4s_inplace(v, r, tmp);
        }
        else if constexpr (NumElements == 8)
        {
            trn8s_inplace(v, r, tmp);
        }
        else if constexpr (NumElements == 16)
        {
            trn16s_inplace(v, r, tmp);
        }
    }

    template <unsigned ElementSize, unsigned NumElements>
    void
    transpose_onto(vreg_view<ElementSize, NumElements>                     v,
                   sysml::miltuple<xbyak::VReg const&, NumElements> const& src,
                   sysml::miltuple<xbyak::VReg const&, NumElements> const& dst)
    {
        static_assert(NumElements == 2 || NumElements == 4 ||
                      NumElements == 8 || NumElements == 16);

        if constexpr (NumElements == 2)
        {
            trn2s_onto(v, src, dst);
        }
        else if constexpr (NumElements == 4)
        {
            trn4s_onto(v, src, dst);
        }
        else if constexpr (NumElements == 8)
        {
            trn8s_onto(v, src, dst);
        }
        else if constexpr (NumElements == 16)
        {
            trn16s_onto(v, src, dst);
        }
    }
};

} // namespace sysml::code_generator::meta_mnemonics
