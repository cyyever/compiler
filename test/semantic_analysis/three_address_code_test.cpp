/*!
 * \file three_address_code_test.cpp
 *
 * \brief
 */
#include <cyy/computation/lang/common_tokens.hpp>
#include <doctest/doctest.h>

#include "exception.hpp"
#include "semantic_analysis/three_address_code_sdd.hpp"

using namespace cyy::computation;
using namespace cyy::compiler;

TEST_CASE("three address code") {
  three_address_code_SDD sdd;

  SUBCASE("translation") {
    std::vector<token> tokens;
    tokens.emplace_back(static_cast<symbol_type>(common_token::id), "a");
    tokens.emplace_back('=', "=");
    tokens.emplace_back(static_cast<symbol_type>(common_token::id), "b");
    tokens.emplace_back('+', "+");
    tokens.emplace_back('-', "-");
    tokens.emplace_back(static_cast<symbol_type>(common_token::id), "c");

    REQUIRE(sdd.run(tokens));
    REQUIRE(!sdd.instruction_sequence.empty());
    for (auto const &instruction : sdd.instruction_sequence) {
      std::cout << instruction->to_string() << std::endl;
    }
  }
}
