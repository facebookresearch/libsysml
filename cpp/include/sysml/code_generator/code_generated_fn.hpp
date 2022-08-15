// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "sysml/memory.hpp"

#include <fstream>    // for std::ofstream
#include <functional> // for std::function
#include <memory>     // for std::shared_ptr
#include <optional>   // for std::optional, std::nullopt
#include <string>     // for std::string
#include <utility>    // for std::exchange

#include "sysml/code_generator/predef.hpp"

#if defined(SYSML_CODE_GENERATOR_ARCHITECTURE_AMD64)
#    define SYSML_CODE_GENERATOR_HAS_PERF_PROFILER
#    include "sysml/code_generator/x86/codegen_perf.hpp"
#endif

namespace sysml::code_generator
{

template <class SignatureTo>
struct dynamic_fn_cast_type;

template <class Signature>
class weak_dynamic_fn;

template <class Signature>
class shared_dynamic_fn;

template <class Ret, class... Args>
class shared_dynamic_fn<Ret(Args...)>
{
private:
    using underlying_pointer    = std::shared_ptr<void>;
    using size_type             = unsigned;
    using function_pointer_type = Ret (*)(Args...);

private:
    underlying_pointer      ptr_  = nullptr;
    std::optional<unsigned> size_ = std::nullopt;

public:
    using weak_type = weak_dynamic_fn<Ret(Args...)>;

public:
    // Constructors
    template <class Deleter>
    shared_dynamic_fn(void* ptr, Deleter&& deleter,
                      std::optional<unsigned> size = std::nullopt) noexcept
        : ptr_(ptr, std::forward<Deleter>(deleter))
        , size_(size)
    {
    }
    // Erased b/c of the declaration above
    shared_dynamic_fn() noexcept = default;

    // copy/move constructors/assignment operators default generated.

public:
    function_pointer_type get() const noexcept
    {
        return reinterpret_cast<function_pointer_type>(ptr_.get());
    }

    Ret operator()(Args... args) const noexcept { return get()(args...); }

    explicit operator bool() const noexcept { return !!ptr_; }

    void swap(shared_dynamic_fn& other) noexcept
    {
        ptr_.swap(other.ptr_);
        size_.swap(other.size_);
    }

    void reset() noexcept
    {
        ptr_.reset();
        size_ = std::nullopt;
    }

    void save_to_file(std::string const& fname)
    {
        if (ptr_ && size_)
        {
            std::ofstream fout(fname.c_str(), std::ios::out | std::ios::binary |
                                                  std::ios::trunc);
            fout.write(reinterpret_cast<char*>(ptr_.get()), *size_);
        }
    }

#ifdef SYSML_CODE_GENERATOR_HAS_PERF_PROFILER
    void register_perf(std::string const& name = "")
    {
        if (ptr_ && size_)
        {
            get_x86_profiler().set(name.c_str(), ptr_.get(),
                                   static_cast<int>(*size_));
        }
    }
#else
    void register_perf(std::string const&) {}
#endif

private: // Weak dynamic fn support
    friend weak_dynamic_fn<Ret(Args...)>;

    shared_dynamic_fn(underlying_pointer const&      ptr,
                      std::optional<unsigned> const& size)
        : ptr_(ptr)
        , size_(size)
    {
    }

private: // Casting support
    template <class>
    friend struct dynamic_fn_cast_type;
};

template <class Signature>
class weak_dynamic_fn;

template <class Ret, class... Args>
class weak_dynamic_fn<Ret(Args...)>
{
private:
    using underlying_pointer = std::weak_ptr<void>;

private:
    underlying_pointer      ptr_{};
    std::optional<unsigned> size_ = std::nullopt;

public:
    using shared_type = shared_dynamic_fn<Ret(Args...)>;

public:
    // Constructors
    weak_dynamic_fn(shared_type const& shared) noexcept
        : ptr_(shared.ptr_)
        , size_(shared.size_)
    {
    }
    // Erased b/c of the declaration above
    weak_dynamic_fn() noexcept = default;

    // copy/move constructors/assignment operators default generated.

    weak_dynamic_fn& operator=(shared_type const& shared) noexcept
    {
        ptr_  = shared.ptr_;
        size_ = shared.size_;
        return *this;
    }

public:
    void swap(weak_dynamic_fn& other) noexcept
    {
        ptr_.swap(other.ptr_);
        size_.swap(other.size_);
    }

    void reset() noexcept
    {
        ptr_.reset();
        size_ = std::nullopt;
    }

    shared_type lock() const noexcept
    {
        return shared_type(ptr_.lock(), size_);
    }
};

template <class Signature>
class unique_dynamic_fn;

template <class Ret, class... Args>
class unique_dynamic_fn<Ret(Args...)>
{
private:
    using underlying_pointer =
        std::unique_ptr<void, std::function<void(void*)>>;
    using size_type             = unsigned;
    using function_pointer_type = Ret (*)(Args...);

private:
    underlying_pointer      ptr_  = nullptr;
    std::optional<unsigned> size_ = std::nullopt;

public:
    // Constructors
    unique_dynamic_fn(void* ptr, std::function<void(void*)> const& deleter,
                      std::optional<unsigned> size = std::nullopt) noexcept
        : ptr_(ptr, deleter)
        , size_(size)
    {
    }
    // Erased b/c of the declaration above
    unique_dynamic_fn() noexcept = default;

    // copy/move constructors/assignment operators default generated.

public:
    function_pointer_type get() const noexcept
    {
        return reinterpret_cast<function_pointer_type>(ptr_.get());
    }

    Ret operator()(Args... args) const noexcept { return get()(args...); }

    explicit operator bool() const noexcept { return !!ptr_; }

    void swap(unique_dynamic_fn& other) noexcept
    {
        ptr_.swap(other.ptr_);
        size_.swap(other.size_);
    }

    void reset() noexcept
    {
        ptr_.reset();
        size_ = std::nullopt;
    }

    void save_to_file(std::string const& fname)
    {
        if (ptr_ && size_)
        {
            std::ofstream fout(fname.c_str(), std::ios::out | std::ios::binary |
                                                  std::ios::trunc);
            fout.write(reinterpret_cast<char*>(ptr_.get()), *size_);
        }
    }

#ifdef SYSML_CODE_GENERATOR_HAS_PERF_PROFILER
    void register_perf(std::string const& name = "")
    {
        if (ptr_ && size_)
        {
            get_x86_profiler().set(name.c_str(), ptr_.get(),
                                   static_cast<int>(*size_));
        }
    }
#else
    void register_perf(std::string const&) {}
#endif

private: // Casting support
    template <class>
    friend struct dynamic_fn_cast_type;

    unique_dynamic_fn(underlying_pointer&&           ptr,
                      std::optional<unsigned> const& size)
        : ptr_(std::move(ptr))
        , size_(size)
    {
    }
};

template <class Signature>
class observed_dynamic_fn;

template <class Ret, class... Args>
class observed_dynamic_fn<Ret(Args...)>
{
private:
    using underlying_pointer    = sysml::observed_ptr<void>;
    using size_type             = unsigned;
    using function_pointer_type = Ret (*)(Args...);

private:
    underlying_pointer      ptr_  = nullptr;
    std::optional<unsigned> size_ = std::nullopt;

public:
    // Constructors
    explicit observed_dynamic_fn(
        void* ptr, std::optional<unsigned> size = std::nullopt) noexcept
        : ptr_(ptr)
        , size_(size)
    {
    }
    // Erased b/c of the declaration above
    observed_dynamic_fn() noexcept = default;

    // copy/move constructors/assignment operators default generated.

public:
    function_pointer_type get() const noexcept
    {
        return reinterpret_cast<function_pointer_type>(ptr_.get());
    }

    Ret operator()(Args... args) const noexcept { return get()(args...); }

    explicit operator bool() const noexcept { return !!ptr_; }

    void swap(observed_dynamic_fn& other) noexcept
    {
        ptr_.swap(other.ptr_);
        size_.swap(other.size_);
    }

    void reset() noexcept
    {
        ptr_.reset();
        size_ = std::nullopt;
    }

    void save_to_file(std::string const& fname)
    {
        if (ptr_ && size_)
        {
            std::ofstream fout(fname.c_str(), std::ios::out | std::ios::binary |
                                                  std::ios::trunc);
            fout.write(reinterpret_cast<char*>(ptr_.get()), *size_);
        }
    }

#ifdef SYSML_CODE_GENERATOR_HAS_PERF_PROFILER
    void register_perf(std::string const& name = "")
    {
        if (ptr_ && size_)
        {
            get_x86_profiler().set(name.c_str(), ptr_.get(),
                                   static_cast<int>(*size_));
        }
    }
#else
    void register_perf(std::string const&) {}
#endif

private: // Casting support
    template <class>
    friend struct dynamic_fn_cast_type;

    observed_dynamic_fn(underlying_pointer const&      ptr,
                        std::optional<unsigned> const& size)
        : ptr_(ptr)
        , size_(size)
    {
    }
};

// Casting support
template <class SignatureTo>
struct dynamic_fn_cast_type
{
    template <class SignatureFrom>
    shared_dynamic_fn<SignatureTo>
    operator()(shared_dynamic_fn<SignatureFrom> const& from) const noexcept
    {
        return shared_dynamic_fn<SignatureTo>(from.ptr_, from.size_);
    }

    template <class SignatureFrom>
    unique_dynamic_fn<SignatureTo>
    operator()(unique_dynamic_fn<SignatureFrom>&& from) const noexcept
    {
        return unique_dynamic_fn<SignatureTo>(std::move(from.ptr_), from.size_);
    }

    template <class SignatureFrom>
    observed_dynamic_fn<SignatureTo>
    operator()(observed_dynamic_fn<SignatureFrom> const& from) const noexcept
    {
        return observed_dynamic_fn<SignatureTo>(from.ptr_, from.size_);
    }
};

template <class To, class RetFrom, class... ArgsFrom>
decltype(auto)
dynamic_fn_cast(shared_dynamic_fn<RetFrom(ArgsFrom...)> const& from) noexcept
{
    dynamic_fn_cast_type<To> caster;
    return caster(from);
}

template <class To, class RetFrom, class... ArgsFrom>
decltype(auto)
dynamic_fn_cast(observed_dynamic_fn<RetFrom(ArgsFrom...)> const& from) noexcept
{
    dynamic_fn_cast_type<To> caster;
    return caster(from);
}

template <class To, class RetFrom, class... ArgsFrom>
decltype(auto)
dynamic_fn_cast(unique_dynamic_fn<RetFrom(ArgsFrom...)>&& from) noexcept
{
    dynamic_fn_cast_type<To> caster;
    return caster(std::move(from));
}

// TODO(zi) remove
template <class To, class From>
decltype(auto) code_generated_fn_cast(From&& from)
{
    return dynamic_fn_cast<To>(std::forward<From>(from));
}

} // namespace sysml::code_generator

#undef SYSML_CODE_GENERATOR_HAS_PERF_PROFILER
