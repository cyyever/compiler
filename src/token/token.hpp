/*!
 * \file token.hpp
 *
 * \author cyy
 */

#pragma once


#include <cyy/computation/lang/lang.hpp>

namespace cyy::compiler {
using namespace cyy::computation;

  struct token_attribute {
    size_t line_no{1};
    size_t column_no{1};
  };

  struct token {
    symbol_type name{};
    symbol_string lexeme;
    token_attribute attribute;
  };

using token_istream = std::basic_istream<token>;
using token_string = std::basic_string<token>;
using token_string_view = std::basic_string_view<token>;
} // namespace cyy::compiler
