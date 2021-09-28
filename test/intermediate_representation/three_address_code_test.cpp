/*!
 * \file three_address_code_test.cpp
 *
 * \brief
 */
#include <cyy/computation/lang/common_tokens.hpp>
#include <doctest/doctest.h>

#include "example_grammar/declaration_sdd.hpp"
#include "example_grammar/expression_three_address_code_sdd.hpp"
#include "example_grammar/lexical_analyzer.hpp"
#include "exception.hpp"
#include "type_expression.hpp"

using namespace cyy::computation;
using namespace cyy::compiler;

TEST_CASE("three address code") {
  example_grammar::expression_three_address_code_SDD sdd;
  example_grammar::declaration_SDD declaration_sdd;
  auto analyzer = example_grammar::get_lexical_analyzer();

  SUBCASE("expression") {
    analyzer->set_source_code("int a;int b;int c;");

    auto declaration_tokens = analyzer->scan_all();
    auto table = declaration_sdd.run(declaration_tokens);

    analyzer->set_source_code("a=b+-c;");
    auto tokens = analyzer->scan_all();

    REQUIRE(sdd.run(tokens, table));
    REQUIRE(!sdd.instruction_sequence.empty());
    for (auto const &instruction : sdd.instruction_sequence) {
      std::cout << instruction->to_string() << std::endl;
    }
  }
  SUBCASE("array reference1") {
    analyzer->set_source_code("int i;int j;int c;int[2][3] a;");
    auto declaration_tokens = analyzer->scan_all();
    auto table = declaration_sdd.run(declaration_tokens);
    REQUIRE(table);
    auto e = table->get_symbol("c");

    REQUIRE(e);

    analyzer->set_source_code("i=1;j=2;c=3;c+a[i][j];");
    auto tokens = analyzer->scan_all();
    REQUIRE(sdd.run(tokens, table));
    REQUIRE(!sdd.instruction_sequence.empty());
    for (auto const &instruction : sdd.instruction_sequence) {
      std::cout << instruction->to_string() << std::endl;
    }
  }
  SUBCASE("array reference2") {

    analyzer->set_source_code(
        "int x;int i;int j;int k;int[10][10] a;int[9][9] b;int[8] c;");
    auto declaration_tokens = analyzer->scan_all();
    auto table = declaration_sdd.run(declaration_tokens);
    REQUIRE(table);

    analyzer->set_source_code("x+a[b[i][j]][c[k]];");
    auto tokens = analyzer->scan_all();

    REQUIRE(sdd.run(tokens, table));
    REQUIRE(!sdd.instruction_sequence.empty());
    for (auto const &instruction : sdd.instruction_sequence) {
      std::cout << instruction->to_string() << std::endl;
    }
  }
}
