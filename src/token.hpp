/*!
 * \file token.hpp
 *
 * \author cyy
 */

#pragma once

#include <cyy/computation/lang/symbol.hpp>
#include <span>

namespace cyy::compiler {
using namespace cyy::computation;

struct token_attribute {
  size_t line_no{1};
  size_t column_no{1};
};

struct token {
  symbol_type name{};
  std::string lexeme;
  token_attribute attribute{};
};


using token_istream = std::basic_istream<token>;
using token_span = std::span<token>;
} // namespace cyy::compiler
