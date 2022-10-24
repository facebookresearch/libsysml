// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/assert.hpp"
#include "sysml/code_generator/xbyak.hpp"

namespace sysml::code_generator::meta_mnemonics
{

template <unsigned ElementSize, unsigned NumElements = 16 / ElementSize>
struct vreg_view
{
private:
    static_assert(ElementSize == 1 || ElementSize == 2 || ElementSize == 4 ||
                  ElementSize == 8);

public:
    decltype(auto) operator[](xbyak::VReg const& r) const noexcept
    {
        if constexpr (ElementSize == 1)
        {
            if constexpr (NumElements == 4 || NumElements == 2)
            {
                return r.b8;
            }
            else if constexpr (NumElements == 8)
            {
                return r.b8;
            }
            else if constexpr (NumElements == 16)
            {
                return r.b16;
            }
            else
            {
                SYSML_STRONG_ASSERT(false);
                return nullptr;
            }
        }
        else if constexpr (ElementSize == 2)
        {
            if constexpr (NumElements == 2)
            {
                return r.h4;
            }
            else if constexpr (NumElements == 4)
            {
                return r.h4;
            }
            else if constexpr (NumElements == 8)
            {
                return r.h8;
            }
            else
            {
                SYSML_STRONG_ASSERT(false);
                return nullptr;
            }
        }
        else if constexpr (ElementSize == 4)
        {
            // if constexpr (NumElements == 1)
            // {
            //     return r.s1;
            // }
            // else
            if constexpr (NumElements == 2)
            {
                return r.s2;
            }
            else if constexpr (NumElements == 4)
            {
                return r.s4;
            }
            else
            {
                SYSML_STRONG_ASSERT(false);
                return nullptr;
            }
        }
        else if constexpr (ElementSize == 8)
        {
            if constexpr (NumElements == 1)
            {
                return r.d1;
            }
            else if constexpr (NumElements == 2)
            {
                return r.d2;
            }
            else
            {
                SYSML_STRONG_ASSERT(false);
                return nullptr;
            }
        }
        else
        {
            SYSML_STRONG_ASSERT(false);
            return nullptr;
        }
    }

    decltype(auto) to_simd_scalar(xbyak::VReg const& r) const noexcept
    {
        if constexpr (ElementSize == 1)
        {
            if constexpr (NumElements == 1)
            {
                return xbyak::BReg(r.getIdx());
            }
            else if constexpr (NumElements == 2)
            {
                return xbyak::HReg(r.getIdx());
            }
            else if constexpr (NumElements == 4)
            {
                return xbyak::SReg(r.getIdx());
            }
            else if constexpr (NumElements == 8)
            {
                return xbyak::DReg(r.getIdx());
            }
            else if constexpr (NumElements == 16)
            {
                return xbyak::QReg(r.getIdx());
            }
            else
            {
                SYSML_STRONG_ASSERT(false);
                return nullptr;
            }
        }
        else if constexpr (ElementSize == 2)
        {
            if constexpr (NumElements == 1)
            {
                return xbyak::HReg(r.getIdx());
            }
            else if constexpr (NumElements == 2)
            {
                return xbyak::SReg(r.getIdx());
            }
            else if constexpr (NumElements == 4)
            {
                return xbyak::DReg(r.getIdx());
            }
            else if constexpr (NumElements == 8)
            {
                return xbyak::QReg(r.getIdx());
            }
            else
            {
                SYSML_STRONG_ASSERT(false);
                return nullptr;
            }
        }
        else if constexpr (ElementSize == 4)
        {
            if constexpr (NumElements == 1)
            {
                return xbyak::SReg(r.getIdx());
            }
            else if constexpr (NumElements == 2)
            {
                return xbyak::DReg(r.getIdx());
            }
            else if constexpr (NumElements == 4)
            {
                return xbyak::QReg(r.getIdx());
            }
            else
            {
                SYSML_STRONG_ASSERT(false);
                return nullptr;
            }
        }
        else if constexpr (ElementSize == 8)
        {
            if constexpr (NumElements == 1)
            {
                return xbyak::DReg(r.getIdx());
            }
            else if constexpr (NumElements == 2)
            {
                return xbyak::QReg(r.getIdx());
            }
            else
            {
                SYSML_STRONG_ASSERT(false);
                return nullptr;
            }
        }
        else if constexpr (ElementSize == 16)
        {
            if constexpr (NumElements == 1)
            {
                return xbyak::QReg(r.getIdx());
            }
            else
            {
                SYSML_STRONG_ASSERT(false);
                return nullptr;
            }
        }
        else
        {
            SYSML_STRONG_ASSERT(false);
            return nullptr;
        }
    }
};

} // namespace sysml::code_generator::meta_mnemonics
