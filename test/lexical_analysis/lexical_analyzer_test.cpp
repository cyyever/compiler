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

#include "../../src/lexical_analysis/lexical_analyzer.hpp"
#include <cyy/computation/lang/common_tokens.hpp>

using namespace cyy::computation;
using namespace cyy::compiler;

TEST_CASE("scan") {
  lexical_analyzer analyzer("common_tokens");

  analyzer.append_pattern(static_cast<symbol_type>(common_token::id),
                          U"[a-zA-Z_][a-zA-Z_0-9]*");
  analyzer.append_pattern(static_cast<symbol_type>(common_token::digit),
                          U"[0-9]+");
  analyzer.append_pattern('+', U"\\+");
  analyzer.append_pattern('*', U"\\*");
  analyzer.append_pattern('=', U"=");

  analyzer.set_input_stream(symbol_istringstream(U"aaaa"));

  auto res = analyzer.scan();
}
