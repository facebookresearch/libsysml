// Copyright (c) Meta Platforms, Inc. and affiliates. All Rights Reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#if defined(META_MNEMONICS_TEST_LIVE_INCLUDE)

#    include <catch2/catch.hpp>

#    include "sysml/code_generator.hpp"
#    include "sysml/code_generator/meta_mnemonics.hpp"

#    include <algorithm>
#    include <cstdint>
#    include <numeric>
#    include <vector>

TEST_CASE("meta_mnemonics::stack", "[push/pop]")
{
    {
        class reverser
            : public sysml::code_generator::code_generator<void(
                  std::uint64_t const*, std::uint64_t*)>,
              public sysml::code_generator::meta_mnemonics::stack<reverser>
        {
        private:
            using stack_mm =
                sysml::code_generator::meta_mnemonics::stack<reverser>;

        public:
            reverser(int len, bool paired = false)
                : stack_mm(x4)
            {
                paired = paired && (len % 2 == 0);

                stack_mm::prologue();

                for (int i = 0; i < len; ++i)
                {
                    ldr(x2, ptr(x0));
                    add(x0, x0, 8);

                    if (paired)
                    {
                        ldr(x3, ptr(x0));
                        add(x0, x0, 8);
                        stack_mm::meta_push_pair(x2, x3);
                        ++i;
                    }
                    else
                    {
                        stack_mm::meta_push(x2);
                    }
                }

                for (int i = 0; i < len; ++i)
                {
                    if (paired)
                    {
                        stack_mm::meta_pop_pair(x2, x3);
                        str(x3, ptr(x1));
                        add(x1, x1, 8);
                        str(x2, ptr(x1));
                        add(x1, x1, 8);
                        ++i;
                    }
                    else
                    {
                        stack_mm::meta_pop(x2);
                        str(x2, ptr(x1));
                        add(x1, x1, 8);
                    }
                }

                stack_mm::epilogue();

                ret();
            }
        };


        std::vector<std::uint64_t> in(10);

        std::iota(in.begin(), in.end(), 23);

        std::vector<std::uint64_t> gold = in;
        std::reverse(gold.begin(), gold.end());

        {
            auto fn = reverser(10).get_unique();
            std::vector<std::uint64_t> out(10);
            fn(in.data(), out.data());
            CHECK(out == gold);
        }

        {
            auto fn = reverser(10, true).get_unique();
            std::vector<std::uint64_t> out(10);
            fn(in.data(), out.data());
            CHECK(out == gold);
        }

        CHECK(3 == 3);
    }
}

#else
#    error "Can't include this file"
#endif
