/*!
 * \file declaration_sdd.cpp
 *
 * \brief
 */

#include "declaration_sdd.hpp"

#include <cyy/computation/lang/common_tokens.hpp>

#include "grammar.hpp"

namespace cyy::compiler::example_grammar {
  declaration_SDD::declaration_SDD()
      : grammar(get_declaration_grammar()),
        table(std::make_shared<symbol_table>()) {}

  bool declaration_SDD::run(token_span span) {
    return sdd->run(span, {}).has_value();
  }
} // namespace cyy::compiler::example_grammar
