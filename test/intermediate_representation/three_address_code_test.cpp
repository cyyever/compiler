/*!
 * \file three_address_code_test.cpp
 *
 * \brief
 */
#include <cyy/computation/lang/common_tokens.hpp>
#include <doctest/doctest.h>

#include "example_grammar/declaration_sdd.hpp"
#include "example_grammar/expression_three_address_code_sdd.hpp"
#include "exception.hpp"
#include "type_expression.hpp"

using namespace cyy::computation;
using namespace cyy::compiler;

TEST_CASE("three address code") {
  example_grammar::expression_three_address_code_SDD sdd;
  example_grammar::declaration_SDD declaration_sdd;

  SUBCASE("expression") {

    std::vector<token> declaration_tokens;
    declaration_tokens.emplace_back(static_cast<symbol_type>(common_token::INT),
                                    "int");
    declaration_tokens.emplace_back(static_cast<symbol_type>(common_token::id),
                                    "a");
    declaration_tokens.emplace_back(';', ";");
    declaration_tokens.emplace_back(static_cast<symbol_type>(common_token::INT),
                                    "int");
    declaration_tokens.emplace_back(static_cast<symbol_type>(common_token::id),
                                    "b");
    declaration_tokens.emplace_back(';', ";");
    declaration_tokens.emplace_back(static_cast<symbol_type>(common_token::INT),
                                    "int");
    declaration_tokens.emplace_back(static_cast<symbol_type>(common_token::id),
                                    "c");
    declaration_tokens.emplace_back(';', ";");

    auto table = declaration_sdd.run(declaration_tokens);

    std::vector<token> tokens;
    tokens.emplace_back(static_cast<symbol_type>(common_token::id), "a");
    tokens.emplace_back('=', "=");
    tokens.emplace_back(static_cast<symbol_type>(common_token::id), "b");
    tokens.emplace_back('+', "+");
    tokens.emplace_back('-', "-");
    tokens.emplace_back(static_cast<symbol_type>(common_token::id), "c");
    tokens.emplace_back(';', ";");

    REQUIRE(sdd.run(tokens, table));
    REQUIRE(!sdd.instruction_sequence.empty());
    for (auto const &instruction : sdd.instruction_sequence) {
      std::cout << instruction->to_string() << std::endl;
    }
  }
  SUBCASE("array reference") {

    std::vector<token> declaration_tokens;
    declaration_tokens.emplace_back(static_cast<symbol_type>(common_token::INT),
                                    "int");
    declaration_tokens.emplace_back(static_cast<symbol_type>(common_token::id),
                                    "i");
    declaration_tokens.emplace_back(';', ";");
    declaration_tokens.emplace_back(static_cast<symbol_type>(common_token::INT),
                                    "int");
    declaration_tokens.emplace_back(static_cast<symbol_type>(common_token::id),
                                    "j");
    declaration_tokens.emplace_back(';', ";");
    declaration_tokens.emplace_back(static_cast<symbol_type>(common_token::INT),
                                    "int");
    declaration_tokens.emplace_back('[', "[");
    declaration_tokens.emplace_back(
        static_cast<symbol_type>(common_token::number), "2");
    declaration_tokens.emplace_back(']', "]");
    declaration_tokens.emplace_back('[', "[");
    declaration_tokens.emplace_back(
        static_cast<symbol_type>(common_token::number), "3");
    declaration_tokens.emplace_back(']', "]");
    declaration_tokens.emplace_back(static_cast<symbol_type>(common_token::id),
                                    "c");
    declaration_tokens.emplace_back(';', ";");

    auto table = declaration_sdd.run(declaration_tokens);
    REQUIRE(table);
    auto e = table->get_symbol("c");

    REQUIRE(e);

    std::vector<token> tokens;
    tokens.emplace_back(static_cast<symbol_type>(common_token::id), "i");
    tokens.emplace_back('=', "=");
    tokens.emplace_back(static_cast<symbol_type>(common_token::number), "1");
    tokens.emplace_back(';', ";");
    tokens.emplace_back(static_cast<symbol_type>(common_token::id), "j");
    tokens.emplace_back('=', "=");
    tokens.emplace_back(static_cast<symbol_type>(common_token::number), "2");
    tokens.emplace_back(';', ";");
    REQUIRE(sdd.run(tokens, table));
    REQUIRE(!sdd.instruction_sequence.empty());
    for (auto const &instruction : sdd.instruction_sequence) {
      std::cout << instruction->to_string() << std::endl;
    }

    /*
      auto table= std::make_shared<cyy::compiler::symbol_table>();
      cyy::compiler::symbol_table::symbol_entry e;
      e.lexeme="a";
      table->add_symbol(e);
    std::vector<token> declaration_tokens;
    declaration_tokens.emplace_back(static_cast<symbol_type>(common_token::id),
    "a"); declaration_tokens.emplace_back('=', "=");
    declaration_tokens.emplace_back(static_cast<symbol_type>(common_token::id),
    "b"); declaration_tokens.emplace_back('+', "+");
    declaration_tokens.emplace_back('-', "-");
    declaration_tokens.emplace_back(static_cast<symbol_type>(common_token::id),
    "c");

    REQUIRE(sdd.run(declaration_tokens,table));
    REQUIRE(!sdd.instruction_sequence.empty());
    for (auto const &instruction : sdd.instruction_sequence) {
      std::cout << instruction->to_string() << std::endl;
    }
    */
  }
}
