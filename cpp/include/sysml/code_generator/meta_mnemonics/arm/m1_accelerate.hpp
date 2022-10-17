// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/assert.hpp"
#include "sysml/code_generator/xbyak.hpp"

#include <cstdint>

namespace sysml::code_generator::meta_mnemonics
{

// Bunch from: https://gist.github.com/dougallj/7a75a3be1ec69ca550e7c36dc75e0d6f
template <class CodeGenerator>
class m1_accelerate
{
private:
    CodeGenerator& self() { return static_cast<CodeGenerator&>(*this); }

public:
    static constexpr std::uint32_t AMX_BASE = 0x00201000;

    static std::uint64_t build_amx_instruction(std::uint32_t op,
                                               std::uint32_t operand) noexcept
    {
        return (AMX_BASE | ((op & 0x1F) << 5) | (operand & 0x1F));
    }

    void issue_amx_instruction(std::uint32_t op, std::uint32_t operand)
    {
        self().dw(build_amx_instruction(op, operand));
    }

    void issue_amx_instruction(std::uint32_t op, Xbyak::XReg const& r)
    {
        issue_amx_instruction(op, static_cast<std::uint32_t>(r.getIdx()));
    }

    void enable_amx()
    {
        self().nop();
        self().nop();
        self().nop();
        issue_amx_instruction(17, 0);
    }

    void disable_amx()
    {
        self().nop();
        self().nop();
        self().nop();
        issue_amx_instruction(17, 1);
    }

    void amxldx(Xbyak::XReg const& r) { issue_amx_instruction(0u, r); }
    void amxldy(Xbyak::XReg const& r) { issue_amx_instruction(1u, r); }
    void amxstx(Xbyak::XReg const& r) { issue_amx_instruction(2u, r); }
    void amxsty(Xbyak::XReg const& r) { issue_amx_instruction(3u, r); }
    void amxldz(Xbyak::XReg const& r) { issue_amx_instruction(4u, r); }
    void amxstz(Xbyak::XReg const& r) { issue_amx_instruction(5u, r); }
    void amxldzi(Xbyak::XReg const& r) { issue_amx_instruction(6u, r); }
    void amxstzi(Xbyak::XReg const& r) { issue_amx_instruction(7u, r); }

    // TODO(ops 6-9)

    void amxfma64(Xbyak::XReg const& r) { issue_amx_instruction(10u, r); }
    void amxfms64(Xbyak::XReg const& r) { issue_amx_instruction(11u, r); }

    void amxfma32(Xbyak::XReg const& r) { issue_amx_instruction(12u, r); }
    void amxfms32(Xbyak::XReg const& r) { issue_amx_instruction(13u, r); }

    void amxmac16(Xbyak::XReg const& r) { issue_amx_instruction(14u, r); }

    void amxfma16(Xbyak::XReg const& r) { issue_amx_instruction(15u, r); }
    void amxfms16(Xbyak::XReg const& r) { issue_amx_instruction(16u, r); }

    // op17 is enable/disable
};

} // namespace sysml::code_generator::meta_mnemonics
