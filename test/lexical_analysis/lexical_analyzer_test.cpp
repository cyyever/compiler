/*!
 * \file lexical_analyzer_test.cpp
 *
 * \brief 测试lexical_analyzer
 * \author cyy
 * \date 2018-10-28
 */
#if __has_include(<CppCoreCheck\Warnings.h>)
#include <CppCoreCheck\Warnings.h>
#pragma warning(disable : ALL_CPPCORECHECK_WARNINGS)
#endif
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <iostream>
#include <utility>

#include <cyy/computation/lang/common_tokens.hpp>

#include "../../src/lexical_analysis/lexical_analyzer.hpp"

using namespace cyy::computation;
using namespace cyy::compiler;

TEST_CASE("scan") {
  lexical_analyzer analyzer("common_tokens");

  analyzer.append_pattern(static_cast<symbol_type>(common_token::id),
                          U"[a-zA-Z_][a-zA-Z_0-9]*");
  analyzer.append_pattern(static_cast<symbol_type>(common_token::digit),
                          U"[0-9]+");
  analyzer.append_pattern(static_cast<symbol_type>(common_token::whitespace),
                          U"[ \\v\\f\\t\\n\\r\\t]*");
  analyzer.append_pattern('+', U"\\+");
  analyzer.append_pattern('*', U"\\*");
  analyzer.append_pattern('=', U"=");

  std::vector<std::pair<symbol_string, symbol_type>> tokens;
  tokens.emplace_back(U"position", static_cast<symbol_type>(common_token::id));
  tokens.emplace_back(U" ", static_cast<symbol_type>(common_token::whitespace));
  tokens.emplace_back(U"=", '=');
  tokens.emplace_back(U" ", static_cast<symbol_type>(common_token::whitespace));
  tokens.emplace_back(U"initial", static_cast<symbol_type>(common_token::id));
  tokens.emplace_back(U" ", static_cast<symbol_type>(common_token::whitespace));
  tokens.emplace_back(U"+", '+');
  tokens.emplace_back(U" ", static_cast<symbol_type>(common_token::whitespace));
  tokens.emplace_back(U"rate", static_cast<symbol_type>(common_token::id));
  tokens.emplace_back(U" ", static_cast<symbol_type>(common_token::whitespace));
  tokens.emplace_back(U"*", '*');
  tokens.emplace_back(U" ", static_cast<symbol_type>(common_token::whitespace));
  tokens.emplace_back(U"60", static_cast<symbol_type>(common_token::digit));

  symbol_string stmt;
  for (auto const &[lexeme, _] : tokens) {
    stmt += lexeme;
  }

  analyzer.set_input_stream(symbol_istringstream(stmt));

  size_t column_no = 1;
  for (auto const &[lexeme, name] : tokens) {
    auto res = analyzer.scan();
    REQUIRE(res.index() == 0);
    auto token = std::get<0>(res);
    REQUIRE(token.name == name);
    REQUIRE(token.lexeme == lexeme);
    REQUIRE(token.attribute.line_no == 1);
    REQUIRE(token.attribute.column_no == column_no);
    column_no += lexeme.size();
  }
}
