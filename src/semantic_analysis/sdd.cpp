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
                                    semantic_rule rule) {
  if (!cfg.has_production(production)) {
    throw exception::unexisted_production("");
  }

  /*
  auto it = all_rules.find(production);
  if (it != all_rules.end()) {
    if (it->second.count(rule)) {
      throw exception::semantic_rule_confliction(production.first);
    }
  }
  */

  if (rule.result_attribute &&
      !rule.result_attribute.value().belong_to_nonterminal( production.first)) {
    throw exception::unexisted_grammar_symbol_attribute(
        rule.result_attribute.value().get_name());
  }

  size_t terminal_cnt = std::count_if(
      production.second.begin(), production.second.end(),
      [](auto &grammar_symbol) { return grammar_symbol.is_terminal(); });

  for (auto const &argument : rule.arguments) {
    auto terminal_index_opt = argument.get_terminal_index();
    if (terminal_index_opt) {
      if (terminal_index_opt.value() > terminal_cnt) {
        throw exception::unexisted_grammar_symbol_attribute(argument.get_name());
      }
      continue;
    }

    if (!std::any_of(production.second.begin(), production.second.end(),
                     [&argument](auto &grammar_symbol) {
                       return grammar_symbol.is_nonterminal() &&
                            argument.belong_to_nonterminal(
                                  *grammar_symbol.get_nonterminal_ptr());
                     })) {
      throw exception::unexisted_grammar_symbol_attribute(argument.get_name());
    }
  }
  all_rules[production].emplace_back(std::move(rule));
}

} // namespace cyy::compiler
