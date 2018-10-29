/*!
 * \file sdd.cpp
 *
 * \author cyy
 * \date 2018-03-04
 */

#include "sdd.hpp"
#include "../exception.hpp"

namespace cyy::compiler {

void SDD::add_synthesized_attribute(const CFG::production_type &production,
                                    const semantic_rule &rule) {
  if (!cfg->has_production(production)) {
    throw exception::unexisted_production("");
  }

  if (is_attribute_of_grammar_symbol(production.first, rule.attribute)) {
    throw exception::unexisted_grammar_symbol_attribute("");
  }

  for (auto const &argument : rule.arguments) {
    bool flag = false;
    for (auto const &body_grammar_symbol : production.second) {
      if (is_attribute_of_grammar_symbol(body_grammar_symbol, argument)) {
        flag = true;
        break;
      }
    }
    if (!flag && is_attribute_of_grammar_symbol(production.first, argument)) {
      flag = true;
    }
    if (!flag) {
      throw exception::unexisted_grammar_symbol_attribute("");
    }
  }

  attribute_dependency[rule.attribute].insert(
      std::move_iterator(rule.arguments.begin()),
      std::move_iterator(rule.arguments.end()));
}
} // namespace cyy::compiler
