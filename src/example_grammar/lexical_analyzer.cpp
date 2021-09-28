/*!
 * \file lexical_analyzer.cpp
 *
 * \brief
 */

#include "lexical_analyzer.hpp"

#include <string>

#include <cyy/computation/lang/common_tokens.hpp>

namespace cyy::compiler::example_grammar {

  std::shared_ptr<cyy::compiler::lexical_analyzer>
  get_lexical_analyzer(bool ignore_whitespace) {

    std::shared_ptr<cyy::compiler::lexical_analyzer> analyzer =
        std::make_shared<cyy::compiler::lexical_analyzer>("common_tokens");
    auto whitespace_pattern = U"[ \\v\\f\\t\\n\\r\\t]*";
    if (ignore_whitespace) {
      analyzer->add_ignored_pattern(
          static_cast<symbol_type>(common_token::whitespace),
          whitespace_pattern);
    } else {
      analyzer->add_pattern(static_cast<symbol_type>(common_token::whitespace),
                            whitespace_pattern);
    }

    analyzer->add_keyword(static_cast<symbol_type>(common_token::INT), U"int");
    analyzer->add_pattern(static_cast<symbol_type>(common_token::id),
                          U"[a-zA-Z_][a-zA-Z_0-9]*");
    analyzer->add_pattern(static_cast<symbol_type>(common_token::number),
                          U"[0-9]+");
    for (auto c : "!%&*()_+|{}[]-=;") {
      std::u32string nonterminal = U"\\";
      nonterminal.push_back(c);
      analyzer->add_pattern(c, nonterminal);
    }
    return analyzer;
  }
} // namespace cyy::compiler::example_grammar