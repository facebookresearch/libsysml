// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/code_generator/xbyak.hpp"

#include <vector>

namespace sysml::code_generator::meta_mnemonics
{

template <class CodeGenerator>
class stack
{
private:
    xbyak::XReg const stack_register;
    unsigned          stack_size;

private:
    CodeGenerator& self() { return static_cast<CodeGenerator&>(*this); }

public:
    explicit stack(xbyak::XReg const& s, unsigned stack_size = 2048) noexcept
        : stack_register(s)
        , stack_size(stack_size)
    {
    }

    // https://stackoverflow.com/questions/27941220/push-lr-and-pop-lr-in-arm-arch64
    void meta_push(xbyak::XReg const& op)
    {
        self().str(op, xbyak::post_ptr(stack_register, 8));
    }

    void meta_pop(xbyak::XReg const& op)
    {
        self().ldr(op, xbyak::pre_ptr(stack_register, -8));
    }

    void meta_push_pair(xbyak::XReg const& op1, xbyak::XReg const& op2)
    {
        self().stp(op1, op2, xbyak::post_ptr(stack_register, 16));
    }

    void meta_pop_pair(xbyak::XReg const& op1, xbyak::XReg const& op2)
    {
        self().ldp(op1, op2, xbyak::pre_ptr(stack_register, -16));
    }

    void meta_push(std::vector<xbyak::XReg> const& regs)
    {
        for (int i = 1; i < regs.size(); i += 2)
        {
            meta_push_pair(regs[i - 1], regs[i]);
        }

        if (regs.size() % 2)
        {
            meta_push(regs.back());
        }
    }

    void meta_pop(std::vector<xbyak::XReg> const& regs)
    {
        if (regs.size() % 2)
        {
            meta_pop(regs.back());
        }

        for (int i = static_cast<int>(regs.size() - (regs.size() % 2) - 2);
             i >= 0; i -= 2)
        {
            meta_pop_pair(regs[i], regs[i + 1]);
        }
    }

    void prologue()
    {
        for (int i = 0; i < stack_size; i += 1024)
        {
            self().sub(self().sp, self().sp, 1024);
        }
        self().mov(stack_register, self().sp);
        self().stp(self().q8, self().q9, xbyak::post_ptr(stack_register, 64));
        self().stp(self().q10, self().q11, xbyak::post_ptr(stack_register, 64));
        self().stp(self().q12, self().q13, xbyak::post_ptr(stack_register, 64));
        self().stp(self().q14, self().q15, xbyak::post_ptr(stack_register, 64));
    }

    void epilogue()
    {
        self().ldp(self().q14, self().q15, xbyak::pre_ptr(stack_register, -64));
        self().ldp(self().q12, self().q13, xbyak::pre_ptr(stack_register, -64));
        self().ldp(self().q10, self().q11, xbyak::pre_ptr(stack_register, -64));
        self().ldp(self().q8, self().q9, xbyak::pre_ptr(stack_register, -64));

        for (int i = 0; i < stack_size; i += 1024)
        {
            self().add(self().sp, self().sp, 1024);
        }
    }
};

} // namespace sysml::code_generator::meta_mnemonics
