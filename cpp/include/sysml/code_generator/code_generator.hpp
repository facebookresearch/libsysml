// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/code_generator/code_generated_fn.hpp"
#include "sysml/code_generator/memory_resource.hpp"
#include "sysml/code_generator/protect.hpp"
#include "sysml/code_generator/xbyak.hpp"

#include <any>         // for std::any
#include <cassert>     // for assert
#include <cstddef>     // for std::size_t
#include <memory>      // for std::make_shared, std::shared_ptr
#include <set>         // for std::set
#include <stdexcept>   // for std::invalid_argument
#include <type_traits> // for std::type_identity, std::is_base_of
#include <utility>     // for std::move
#include <vector>      // for std::vector

namespace sysml::code_generator
{

class allocator_adapter_base : public xbyak::allocator
{
private:
    memory_resource* resource_;
    std::set<void*>  managed_;

public:
    allocator_adapter_base(memory_resource* resource)
        : resource_(resource)
    {
    }

    ~allocator_adapter_base() { assert(managed_.empty()); }

    xbyak::buffer_type* alloc(std::size_t size) final override
    {
        auto ptr = resource_->allocate_bytes(size * sizeof(xbyak::buffer_type));
        if (!resource_->is_inplace())
        {
            managed_.insert(ptr);
        }
        return reinterpret_cast<xbyak::buffer_type*>(ptr);
    }

    void free(xbyak::buffer_type* ptr) final override
    {
        if (!resource_->is_inplace())
        {
            auto it = managed_.find(ptr);
            if (it != managed_.end())
            {
                resource_->deallocate_bytes(ptr);
                managed_.erase(ptr);
            }
        }
    }

    bool useProtect() const final override { return false; }

    xbyak::buffer_type* release(xbyak::buffer_type* ptr)
    {
        if (!resource_->is_inplace())
        {
            auto it = managed_.find(ptr);
            if (it == managed_.end())
            {
                throw std::invalid_argument(
                    "pointer not managed by the allocator");
            }
            managed_.erase(it);
        }
        return ptr;
    }

    xbyak::allocator* self() { return this; }

    auto get_deleter()
    {
        assert(!resource_->is_inplace());
        return [resource = this->resource_](void* ptr)
        { resource->deallocate_bytes(ptr); };
    }

    bool is_inplace() const { return resource_->is_inplace(); }
};

class basic_code_generator : private allocator_adapter_base,
                             public xbyak::code_generator
{
private:
    template <class T>
    T get_unique_or_shared()
    {
        assert(!allocator_adapter_base::is_inplace());
        ready();
        std::size_t size = getSize() * sizeof(xbyak::buffer_type);
        auto        ptr  = allocator_adapter_base::release(
                    const_cast<xbyak::buffer_type*>(getCode()));

        ::sysml::code_generator::protect(
            ptr, size, ::sysml::code_generator::memory_protection_mode::re);

        return T(
            ptr,
            [deleter = get_deleter(), size](void* p)
            {
                ::sysml::code_generator::protect(
                    p, size,
                    ::sysml::code_generator::memory_protection_mode::rw);
                deleter(p);
            },
            size);
    }

#if defined(SYSML_CODE_GENERATOR_ARCHITECTURE_AARCH64)

protected:
    std::vector<std::any> raii;

public:
    std::shared_ptr<xbyak::label> make_label()
    {
        auto ret = std::make_shared<xbyak::label>();
        raii.push_back(ret);
        return ret;
    }

#endif

public:
#if defined(SYSML_CODE_GENERATOR_ARCHITECTURE_AMD64)
    using Reg64  = xbyak::Reg64;
    using Label  = xbyak::Label;
    using Xmm    = xbyak::Xmm;
    using Ymm    = xbyak::Ymm;
    using Zmm    = xbyak::Zmm;
    using OpMask = xbyak::Opmask;

    auto argument_address(std::size_t N) const
    {
        assert(N > 5);
        return ptr[rsp + (N - 5 * 8)];
    }

    void align_to(unsigned alignment)
    {
        if (getSize() % alignment)
        {
            nop(alignment - static_cast<unsigned>(getSize() % alignment));
        }
    }

#elif defined(SYSML_CODE_GENERATOR_ARCHITECTURE_AARCH64)

    using Reg64 = xbyak::XReg;
    using Label = xbyak::LabelAArch64;
    using VReg  = xbyak::VReg;
    using HReg  = xbyak::HReg;
    using SReg  = xbyak::SReg;
    using DReg  = xbyak::DReg;
    using QReg  = xbyak::QReg;
    using WReg  = xbyak::WReg;
    using XReg  = xbyak::XReg;

#endif

    explicit basic_code_generator(
        memory_resource* resource = memory_resource::default_resource())
        : allocator_adapter_base(resource)
        , xbyak::code_generator(65536, xbyak::AutoGrow,
                                allocator_adapter_base::self())
    {
    }

    template <class Signature>
    unique_dynamic_fn<Signature> get_unique_fn() &&
    {
        return get_unique_or_shared<unique_dynamic_fn<Signature>>();
    }

    template <class Signature>
    shared_dynamic_fn<Signature> get_shared_fn() &&
    {
        return get_unique_or_shared<shared_dynamic_fn<Signature>>();
    }

    template <class Signature>
    observed_dynamic_fn<Signature> get_observed_fn() &&
    {
        assert(allocator_adapter_base::is_inplace());
        ready();
        std::size_t size = getSize() * sizeof(xbyak::buffer_type);
        auto        ptr  = const_cast<xbyak::buffer_type*>(getCode());
        return observed_dynamic_fn<Signature>(ptr, size);
    }
};

template <class>
class code_generator;

template <class Ret, class... Args>
class code_generator<Ret(Args...)> : public basic_code_generator
{
private:
    using signature_type = std::type_identity_t<Ret(Args...)>;

public:
    explicit code_generator(
        memory_resource* resource = memory_resource::default_resource())
        : basic_code_generator(resource)
    {
    }

    unique_dynamic_fn<signature_type> get_unique() &&
    {
        basic_code_generator* base = this;
        return std::move(*base).template get_unique_fn<signature_type>();
    }

    shared_dynamic_fn<signature_type> get_shared() &&
    {
        basic_code_generator* base = this;
        return std::move(*base).template get_shared_fn<signature_type>();
    }

    observed_dynamic_fn<signature_type> get_reference() &&
    {
        basic_code_generator* base = this;
        return std::move(*base).template get_observed_fn<signature_type>();
    }
};

template <class, class>
class with_signature;

template <class BasicCodeGenerator, class Ret, class... Args>
class with_signature<BasicCodeGenerator, Ret(Args...)>
{
private:
    using signature_type = std::type_identity_t<Ret(Args...)>;

    BasicCodeGenerator& self()
    {
        static_assert(
            std::is_base_of_v<basic_code_generator, BasicCodeGenerator>);
        return static_cast<BasicCodeGenerator&>(*this);
    }

public:
    unique_dynamic_fn<signature_type> get_unique() &&
    {
        return std::move(self()).template get_unique_fn<signature_type>();
    }

    shared_dynamic_fn<signature_type> get_shared() &&
    {
        return std::move(self()).template get_shared_fn<signature_type>();
    }

    observed_dynamic_fn<signature_type> get_reference() &&
    {
        return std::move(self()).template get_observed_fn<signature_type>();
    }
};

} // namespace sysml::code_generator
