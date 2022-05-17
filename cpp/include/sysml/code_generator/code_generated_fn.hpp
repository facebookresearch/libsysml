// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include <sysml/code_generator/predef.hpp>
#include <sysml/code_generator/protect.hpp>

#if defined(SYSML_CODE_GENERATOR_ARCHITECTURE_AMD64)
#    define SYSML_CODE_GENERATOR_HAS_PERF_PROFILER
#    include "sysml/code_generator/x86/codegen_perf.hpp"
#endif

#include <cstdint>
#include <fstream>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

namespace sysml::code_generator
{

// Forward decl
class basic_code_generator;

template <typename Signature>
class unique_code_generated_fn;

template <typename Signature>
class weak_code_generated_fn;

template <typename Signature>
class shared_code_generated_fn;

template <typename Signature> // To be used with an inplace allocator
class code_generated_fn_ref;  // that is manually managing the execution
                              // priveleges

struct code_generated_fn_cast_tag
{
};

template <class To, class From>
struct code_generated_fn_caster;

template <class Deleter>
class mprotect_deleter_wrapper
{
private:
    Deleter     deleter_;
    std::size_t size_;

public:
    mprotect_deleter_wrapper() {}
    explicit mprotect_deleter_wrapper(Deleter deleter, std::size_t size)
        : deleter_(deleter)
        , size_(size)
    {
    }

    mprotect_deleter_wrapper(mprotect_deleter_wrapper const&) = default;
    mprotect_deleter_wrapper&
    operator=(mprotect_deleter_wrapper const&) = default;

    void operator()(void* ptr) const
    {
        ::sysml::code_generator::protect(
            ptr, size_, ::sysml::code_generator::memory_protection_mode::rw);
        deleter_(ptr);
    }
};

inline void mprotect_make_executable(void* buffer, std::size_t size)
{
    ::sysml::code_generator::protect(
        buffer, size, ::sysml::code_generator::memory_protection_mode::re);
}

template <typename ReturnType, typename... Args>
class unique_code_generated_fn<ReturnType(Args...)>
{
public:
    using function_pointer_type = ReturnType (*)(Args...);

private:
    using deleter_type = mprotect_deleter_wrapper<std::function<void(void*)>>;

    std::unique_ptr<void, deleter_type> executable_buffer_;
    std::size_t                         size_ = 0;

    friend class basic_code_generator;

    template <typename Deleter>
    unique_code_generated_fn(void* buffer, std::size_t size, Deleter deleter)
        : executable_buffer_(buffer, deleter_type(deleter, size))
        , size_(size)
    {
        ::sysml::code_generator::protect(
            buffer, size, ::sysml::code_generator::memory_protection_mode::re);
    }

    template <class, class>
    friend struct code_generated_fn_caster;

    template <class>
    friend class unique_code_generated_fn;

    template <typename OtherSignature>
    unique_code_generated_fn(unique_code_generated_fn<OtherSignature>&& other,
                             code_generated_fn_cast_tag)
    {
        executable_buffer_ = std::move(other.executable_buffer_);
        size_              = std::exchange(other.size_, 0);
    }

public:
    unique_code_generated_fn() noexcept {}

    unique_code_generated_fn(unique_code_generated_fn const&) = delete;
    unique_code_generated_fn&
    operator=(unique_code_generated_fn const&) = delete;

    unique_code_generated_fn(unique_code_generated_fn&& other) noexcept =
        default;
    unique_code_generated_fn&
    operator=(unique_code_generated_fn&& other) noexcept = default;

    function_pointer_type get() const
    {
        return reinterpret_cast<function_pointer_type>(
            executable_buffer_.get());
    }

    ReturnType operator()(Args... args) const { return this->get()(args...); }

    explicit operator bool() const noexcept { return !!executable_buffer_; }

    void save_to_file(std::string const& fname) const
    {
        std::ofstream fout(fname.c_str(), std::ios::out | std::ios::binary);
        fout.write(reinterpret_cast<char*>(executable_buffer_.get()), size_);
    }

#ifdef SYSML_CODE_GENERATOR_HAS_PERF_PROFILER
    void register_perf(std::string const& name = "")
    {
        get_x86_profiler().set(name.c_str(), executable_buffer_.get(),
                               (int)size_);
    }
#else
    void register_perf(std::string const&) {}
#endif
};

template <typename ReturnType, typename... Args>
class shared_code_generated_fn<ReturnType(Args...)>
{
public:
    using function_pointer_type = ReturnType (*)(Args...);

private:
    std::shared_ptr<void> executable_buffer_;
    std::size_t           size_ = 0;

    friend class weak_code_generated_fn<ReturnType(Args...)>;

    shared_code_generated_fn(std::weak_ptr<void> const& weak_buffer,
                             std::size_t                size)
        : executable_buffer_(weak_buffer.lock())
        , size_(size)
    {
    }

    friend class basic_code_generator;

    template <typename Deleter>
    shared_code_generated_fn(void* buffer, std::size_t size, Deleter deleter)
        : executable_buffer_(buffer,
                             mprotect_deleter_wrapper<Deleter>(deleter, size))
        , size_(size)
    {
        ::sysml::code_generator::protect(
            buffer, size, ::sysml::code_generator::memory_protection_mode::re);
    }

    template <class, class>
    friend struct code_generated_fn_caster;

    template <class>
    friend class shared_code_generated_fn;

    template <typename OtherSignature>
    shared_code_generated_fn(shared_code_generated_fn<OtherSignature>&& other,
                             code_generated_fn_cast_tag)
    {
        executable_buffer_ = std::move(other.executable_buffer_);
        size_              = std::exchange(other.size_, 0);
    }

    template <typename OtherSignature>
    shared_code_generated_fn(
        shared_code_generated_fn<OtherSignature> const& other,
        code_generated_fn_cast_tag)
    {
        executable_buffer_ = other.executable_buffer_;
        size_              = other.size_;
    }

public:
    shared_code_generated_fn() noexcept {}

    shared_code_generated_fn(shared_code_generated_fn const& other) = default;
    shared_code_generated_fn&
    operator=(shared_code_generated_fn const& other)           = default;
    shared_code_generated_fn(shared_code_generated_fn&& other) = default;
    shared_code_generated_fn&
    operator=(shared_code_generated_fn&& other) = default;

    function_pointer_type get() const
    {
        return reinterpret_cast<function_pointer_type>(
            executable_buffer_.get());
    }

    ReturnType operator()(Args... args) const { return this->get()(args...); }

    explicit operator bool() const noexcept { return !!executable_buffer_; }

    void save_to_file(std::string const& fname) const
    {
        std::ofstream fout(fname.c_str(), std::ios::out | std::ios::binary);
        fout.write(reinterpret_cast<char*>(executable_buffer_.get()), size_);
    }

#ifdef SYSML_CODE_GENERATOR_HAS_PERF_PROFILER
    void register_perf(std::string const& name = "")
    {
        get_x86_profiler().set(name.c_str(), executable_buffer_.get(),
                               (int)size_);
    }
#else
    void register_perf(std::string const&) {}
#endif
};

template <typename ReturnType, typename... Args>
class weak_code_generated_fn<ReturnType(Args...)>
{
public:
    using function_pointer_type = ReturnType (*)(Args...);

private:
    std::weak_ptr<void> weak_buffer_;
    std::size_t         size_ = 0;

    using matching_shared_fn = shared_code_generated_fn<ReturnType(Args...)>;

public:
    weak_code_generated_fn() noexcept {}

    weak_code_generated_fn(weak_code_generated_fn const& other) = default;
    weak_code_generated_fn&
    operator=(weak_code_generated_fn const& other)         = default;
    weak_code_generated_fn(weak_code_generated_fn&& other) = default;
    weak_code_generated_fn& operator=(weak_code_generated_fn&& other) = default;

    weak_code_generated_fn(matching_shared_fn const& shared)
        : weak_buffer_(shared.executable_buffer_)
        , size_(shared.size_)
    {
    }

    weak_code_generated_fn& operator=(matching_shared_fn const& shared)
    {
        weak_buffer_ = shared.executable_buffer_;
        return *this;
    }

    matching_shared_fn lock() const noexcept
    {
        return matching_shared_fn(weak_buffer_, size_);
    }
};

template <typename ReturnType, typename... Args>
class code_generated_fn_ref<ReturnType(Args...)>
{
public:
    using function_pointer_type = ReturnType (*)(Args...);

private:
    void*       executable_buffer_ptr_ = nullptr;
    std::size_t size_                  = 0;

    friend class basic_code_generator;

    explicit code_generated_fn_ref(void* buffer, std::size_t size) noexcept
        : executable_buffer_ptr_(buffer)
        , size_(size)
    {
    }

    template <class, class>
    friend struct code_generated_fn_caster;

    template <class>
    friend class code_generated_fn_ref;

    template <typename OtherSignature>
    code_generated_fn_ref(code_generated_fn_ref<OtherSignature> const& other,
                          code_generated_fn_cast_tag)
    {
        executable_buffer_ptr_ = other.executable_buffer_ptr_;
        size_                  = other.size_;
    }

public:
    code_generated_fn_ref() noexcept {}
    code_generated_fn_ref(code_generated_fn_ref const&) noexcept = default;
    code_generated_fn_ref&
    operator=(code_generated_fn_ref const&) noexcept = default;

    code_generated_fn_ref(code_generated_fn_ref&& other) noexcept = default;
    code_generated_fn_ref&
    operator=(code_generated_fn_ref&& other) noexcept = default;

    function_pointer_type get() const noexcept
    {
        return reinterpret_cast<function_pointer_type>(executable_buffer_ptr_);
    }

    ReturnType operator()(Args... args) const { return this->get()(args...); }

    explicit operator bool() const noexcept { return executable_buffer_ptr_; }

    void save_to_file(std::string const& fname) const
    {
        std::ofstream fout(fname.c_str(), std::ios::out | std::ios::binary);
        fout.write(reinterpret_cast<char*>(executable_buffer_ptr_), size_);
    }

#ifdef SYSML_CODE_GENERATOR_HAS_PERF_PROFILER
    void register_perf(std::string const& name = "")
    {
        get_x86_profiler().set(name.c_str(), executable_buffer_ptr_,
                               (int)size_);
    }
#else
    void register_perf(std::string const&) {}
#endif
};

template <class RetTo, class... ArgsTo, class RetFrom, class... ArgsFrom>
struct code_generated_fn_caster<RetTo(ArgsTo...),
                                unique_code_generated_fn<RetFrom(ArgsFrom...)>>
{
    unique_code_generated_fn<RetTo(ArgsTo...)>
    do_cast(unique_code_generated_fn<RetFrom(ArgsFrom...)>&& other)
    {
        return unique_code_generated_fn<RetTo(ArgsTo...)>(
            std::move(other), code_generated_fn_cast_tag());
    }
};

template <class RetTo, class... ArgsTo, class RetFrom, class... ArgsFrom>
struct code_generated_fn_caster<RetTo(ArgsTo...),
                                shared_code_generated_fn<RetFrom(ArgsFrom...)>>
{
    shared_code_generated_fn<RetTo(ArgsTo...)>
    do_cast(shared_code_generated_fn<RetFrom(ArgsFrom...)>&& other)
    {
        return shared_code_generated_fn<RetTo(ArgsTo...)>(
            std::move(other), code_generated_fn_cast_tag());
    }

    shared_code_generated_fn<RetTo(ArgsTo...)>
    do_cast(shared_code_generated_fn<RetFrom(ArgsFrom...)> const& other)
    {
        return shared_code_generated_fn<RetTo(ArgsTo...)>(
            other, code_generated_fn_cast_tag());
    }
};

template <class RetTo, class... ArgsTo, class RetFrom, class... ArgsFrom>
struct code_generated_fn_caster<RetTo(ArgsTo...),
                                code_generated_fn_ref<RetFrom(ArgsFrom...)>>
{
    code_generated_fn_ref<RetTo(ArgsTo...)>
    do_cast(code_generated_fn_ref<RetFrom(ArgsFrom...)> const& other)
    {
        return code_generated_fn_ref<RetTo(ArgsTo...)>(
            other, code_generated_fn_cast_tag());
    }
};

template <class To, class From>
decltype(auto) code_generated_fn_cast(From&& from)
{
    code_generated_fn_caster<To, std::decay_t<From>> caster;
    return caster.do_cast(std::forward<From>(from));
}

} // namespace sysml::code_generator

#undef SYSML_CODE_GENERATOR_HAS_PERF_PROFILER
