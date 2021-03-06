/*!
 * \file lexical_analyzer_test.cpp
 *
 * \brief 测试lexical_analyzer
 * \author cyy
 * \date 2018-10-28
 */
#include <doctest/doctest.h>
#include <utility>

#include <cyy/computation/lang/common_tokens.hpp>

#include "../../src/lexical_analysis/lexical_analyzer.hpp"

using namespace cyy::computation;
using namespace cyy::compiler;

TEST_CASE("scan") {
  lexical_analyzer analyzer("common_tokens");

  analyzer.add_pattern(static_cast<symbol_type>(common_token::id),
                       U"[a-zA-Z_][a-zA-Z_0-9]*");
  analyzer.add_pattern(static_cast<symbol_type>(common_token::number),
                       U"[0-9]+");
  analyzer.add_pattern(static_cast<symbol_type>(common_token::whitespace),
                       U"[ \\v\\f\\t\\n\\r\\t]*");
  analyzer.add_pattern('+', U"\\+");
  analyzer.add_pattern('*', U"\\*");
  analyzer.add_pattern('=', U"=");

  std::vector<std::pair<std::string, symbol_type>> tokens;
  tokens.emplace_back("position", static_cast<symbol_type>(common_token::id));
  tokens.emplace_back(" ", static_cast<symbol_type>(common_token::whitespace));
  tokens.emplace_back("=", '=');
  tokens.emplace_back(" ", static_cast<symbol_type>(common_token::whitespace));
  tokens.emplace_back("initial", static_cast<symbol_type>(common_token::id));
  tokens.emplace_back(" ", static_cast<symbol_type>(common_token::whitespace));
  tokens.emplace_back("+", '+');
  tokens.emplace_back(" ", static_cast<symbol_type>(common_token::whitespace));
  tokens.emplace_back("rate", static_cast<symbol_type>(common_token::id));
  tokens.emplace_back(" ", static_cast<symbol_type>(common_token::whitespace));
  tokens.emplace_back("*", '*');
  tokens.emplace_back(" ", static_cast<symbol_type>(common_token::whitespace));
  tokens.emplace_back("60", static_cast<symbol_type>(common_token::number));

  std::string stmt;
  for (auto const &[lexeme, _] : tokens) {
    stmt += lexeme;
  }

  REQUIRE(analyzer.set_source_code(std::istringstream(stmt)));

  size_t column_no = 1;
  for (auto const &[lexeme, name] : tokens) {
    auto res = analyzer.scan();
    REQUIRE(res.has_value());
    auto const &token = res.value();
    REQUIRE_EQ((int)token.name, (int)name);
    REQUIRE_EQ(token.lexeme, lexeme);
    REQUIRE_EQ(token.attribute.line_no, 1);
    REQUIRE_EQ(token.attribute.column_no, column_no);
    column_no += lexeme.size();
  }
}
