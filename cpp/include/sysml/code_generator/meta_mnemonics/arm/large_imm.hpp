// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/assert.hpp"
#include "sysml/code_generator/xbyak.hpp"

#include <vector>

namespace sysml::code_generator::meta_mnemonics
{

template <class CodeGenerator>
class large_imm
{
private:
    xbyak::XReg const tmp_register;

private:
    CodeGenerator& self() { return static_cast<CodeGenerator&>(*this); }

public:
    explicit large_imm(xbyak::XReg const& t) noexcept
        : tmp_register(t)
    {
    }

private:
    template <class T>
    void meta_add_or_sub_imm(xbyak::XReg const& srcdst, T imm,
                             xbyak::XReg const& tmpreg)
    {
        SYSML_STRONG_ASSERT(srcdst.getIdx() != tmpreg.getIdx());

        if (imm == 0)
        {
            return;
        }

        if (imm > 0)
        {
            self().add_imm(srcdst, srcdst, imm, tmpreg);
        }
        else
        {
            self().sub_imm(srcdst, srcdst, -imm, tmpreg);
        }
    }

    template <typename T>
    void meta_mov_imm(const xbyak::XReg& dst, T imm, const xbyak::XReg& tmp)
    {
        SYSML_STRONG_ASSERT(dst.getIdx() != tmp.getIdx());

        std::int64_t  bit_ptn = static_cast<int64_t>(imm);
        std::uint64_t mask    = 0xFFFF;
        bool          flag    = false;

        /* ADD(immediate) supports unsigned imm12 */
        std::uint64_t const IMM12_MASK = ~uint64_t(0xfff);
        if ((bit_ptn & IMM12_MASK) == 0)
        { // <= 4095
            self().mov(dst, static_cast<uint32_t>(imm & 0xfff));
            return;
        }

        /* MOVZ allows shift amount = 0, 16, 32, 48 */
        for (int i = 0; i < 64; i += 16)
        {
            uint64_t tmp_ptn = (bit_ptn & (mask << i)) >> i;
            if (tmp_ptn)
            {
                if (!flag)
                {
                    self().movz(dst, static_cast<uint32_t>(tmp_ptn), i);
                    flag = true;
                }
                else
                {
                    self().movz(tmp, static_cast<uint32_t>(tmp_ptn), i);
                    self().add(dst, dst, tmp);
                }
            }
        }

        return;
    }

public:
    template <class T>
    void meta_sub_imm(xbyak::XReg const& srcdst, T imm)
    {
        if (imm == 0) return;

        self().sub_imm(srcdst, srcdst, imm, tmp_register);
    }

    template <class T>
    void meta_add_imm(xbyak::XReg const& srcdst, T imm)
    {
        if (imm == 0) return;

        self().add_imm(srcdst, srcdst, imm, tmp_register);
    }

    template <class T>
    void meta_sadd_imm(xbyak::XReg const& srcdst, T imm)
    {
        self().meta_add_or_sub_imm(srcdst, imm, tmp_register);
    }

    void meta_cmp(xbyak::XReg const& xreg, int imm)
    {
        if (imm >= -256 && imm < 256)
        {
            self().cmp(xreg, imm);
        }
        else
        {
            SYSML_STRONG_ASSERT((imm & 0xffff) ==
                                imm); // Add support for larger values
            // meta_mov_imm(tmp_register, imm);
            self().mov(tmp_register, imm);
            self().cmp(xreg, tmp_register);
        }
    }

    template <typename T>
    void meta_mov_imm(const xbyak::XReg& dst, T imm)
    {
        SYSML_STRONG_ASSERT(dst.getIdx() != tmp_register.getIdx());
        meta_mov_imm(dst, imm, tmp_register);
    }
};

} // namespace sysml::code_generator::meta_mnemonics
