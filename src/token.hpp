/*!
 * \file token.hpp
 *
 * \author cyy
 */

#pragma once

#include <span>

#include <cyy/algorithm/alphabet/symbol.hpp>

namespace cyy::compiler {
  using cyy::algorithm::symbol_type;

  struct token_attribute {
    size_t line_no{1};
    size_t column_no{1};
  };

  struct token {
    symbol_type name{};
    std::string lexeme;
    token_attribute attribute{};
  };

  using token_span = std::span<token>;
} // namespace cyy::compiler
