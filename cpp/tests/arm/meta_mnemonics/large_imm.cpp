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

TEST_CASE("meta_mnemonics::large_imm", "[add]")
{
    {
        enum struct operation
        {
            add_imm,
            sub_imm,
            sadd_imm,
            mov_imm
        };

        class adder
            : public sysml::code_generator::code_generator<int(int)>,
              public sysml::code_generator::meta_mnemonics::large_imm<adder>
        {
        private:
            using large_imm =
                sysml::code_generator::meta_mnemonics::large_imm<adder>;

        public:
            adder(int arg, operation op)
                : large_imm(x4)
            {
                switch (op)
                {
                case operation::add_imm:
                    large_imm::meta_add_imm(x0, arg);
                    break;
                case operation::sub_imm:
                    large_imm::meta_sub_imm(x0, arg);
                    break;
                case operation::sadd_imm:
                    large_imm::meta_sadd_imm(x0, arg);
                    break;
                case operation::mov_imm:
                    large_imm::meta_mov_imm(x0, arg);
                }
                ret();
            }
        };

        std::vector<int> nums(10);
        for (int i = 0; i < 10; ++i)
        {
            nums[i] = i * 100'000'000;
        }

        for (auto&& a : nums)
        {

            auto fn_mov = adder(a, operation::mov_imm).get_unique();
            CHECK(fn_mov(-1) == a);

            auto fn_add   = adder(a, operation::add_imm).get_unique();
            auto fn_sub   = adder(a, operation::sub_imm).get_unique();
            auto fn_sadd1 = adder(a, operation::sadd_imm).get_unique();
            auto fn_sadd2 = adder(-a, operation::sadd_imm).get_unique();

            for (auto&& b : nums)
            {
                CHECK(fn_add(b) == a + b);
                CHECK(fn_sub(b) == b - a);
                CHECK(fn_sadd1(b) == b + a);
                CHECK(fn_sadd2(b) == b - a);
            }
        }
    }
}

#else
#    error "Can't include this file"
#endif
