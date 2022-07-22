// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include <cassert>
#include <sstream>
#include <stdexcept>

#define SYSML_ASSERT assert

#define SYSML_STRINGIFY_0(s) #s
#define SYSML_STRINGIFY(s) SYSML_STRINGIFY_0(s)

#define SYSML_CONCAT_0(a, b) a##b
#define SYSML_CONCAT(a, b) SYSML_CONCAT_0(a, b)

#define SYSML_UNIQUE_VARIABLE_NAME(base) SYSML_CONCAT(base, __COUNTER__)

#define SYSML_STRONG_ASSERT(condition)                                         \
    if (!(condition))                                                          \
    {                                                                          \
        throw std::runtime_error(                                              \
            SYSML_STRINGIFY(condition) " failed file: " __FILE__               \
                                       " line: " SYSML_STRINGIFY((__LINE__))); \
    }                                                                          \
    static_cast<void>(0)

namespace sysml::detail::assert
{

struct throw_when_checker
{
};

template <class Exception>
class throw_when
{
private:
    std::ostringstream oss_;
    bool               should_throw_;

public:
    throw_when(bool t)
        : should_throw_(t)
    {
    }

    template <class T>
    throw_when& operator<<(T const& v)
    {
        if (should_throw_)
        {
            oss_ << v;
        }
        return *this;
    }

    operator throw_when_checker()
    {
        if (should_throw_)
        {
            throw Exception(oss_.str());
        }
        return {};
    }
};

} // namespace sysml::detail::assert

#define SYSML_THROW_ASSERT_THIS_EXCEPTION(condition, e)                        \
    [[maybe_unused]] ::sysml::detail::assert::throw_when_checker               \
    SYSML_UNIQUE_VARIABLE_NAME(                                                \
        sysml_throw_assert_this_exception_variable_number_) =                  \
        ::sysml::detail::assert::throw_when<e>(!(condition))                   \
        << "[ file: " __FILE__ " line: " SYSML_STRINGIFY((__LINE__)) "]"

#define SYSML_THROW_ASSERT(condition)                                          \
    SYSML_THROW_ASSERT_THIS_EXCEPTION(condition, ::std::runtime_error)
