/*!
 * \file sdd.cpp
 *
 * \author cyy
 * \date 2018-03-04
 */

#include "sdd.hpp"
#include "../exception.hpp"

namespace cyy::compiler {

void SDD::add_synthesized_attribute(const CFG_production &production,
                                    semantic_rule rule) {
  if (!cfg.has_production(production)) {
    throw exception::unexisted_production("");
  }

  if (!rule.result_attribute) {
    throw exception::no_synthesized_grammar_symbol_attribute(
        "no result_attribute");
  }

  const auto &result_attribute_name = rule.result_attribute.value();
  if (!result_attribute_name.belong_to_nonterminal(production.get_head())) {
    throw exception::unexisted_grammar_symbol_attribute(
        result_attribute_name.get_name());
  }

  check_semantic_rule(production, rule);

  size_t terminal_cnt = std::count_if(
      production.get_body().begin(), production.get_body().end(),
      [](auto &grammar_symbol) { return grammar_symbol.is_terminal(); });

  for (auto const &argument : rule.arguments) {
    auto terminal_index_opt = argument.get_terminal_index();
    if (terminal_index_opt) {
      if (terminal_index_opt.value() > terminal_cnt) {
        throw exception::unexisted_grammar_symbol_attribute(
            argument.get_name());
      }
      continue;
    }

    if (!std::any_of(production.get_body().begin(), production.get_body().end(),
                     [&argument](auto &grammar_symbol) {
                       return grammar_symbol.is_nonterminal() &&
                              argument.belong_to_nonterminal(
                                  *grammar_symbol.get_nonterminal_ptr());
                     })) {
      throw exception::unexisted_grammar_symbol_attribute(argument.get_name());
    }
    synthesized_attribute_dependency[result_attribute_name].insert(argument);
  }

  synthesized_attributes.insert(result_attribute_name);
  all_rules[production].emplace_back(std::move(rule));
}

void SDD::add_inherited_attribute(const CFG_production &production,
                                  semantic_rule rule) {
  if (!cfg.has_production(production)) {
    throw exception::unexisted_production("");
  }

  if (!rule.result_attribute) {
    throw exception::no_inherited_grammar_symbol_attribute(
        "no result_attribute");
  }
  check_semantic_rule(production, rule);

  const auto &result_attribute_name = rule.result_attribute.value();

  // result_attribute must in production body
  auto const result_grammar_it =
      std::find_if(production.get_body().begin(), production.get_body().end(),
                   [&result_attribute_name](auto &grammar_symbol) {
                     return grammar_symbol.is_nonterminal() &&
                            result_attribute_name.belong_to_nonterminal(
                                *grammar_symbol.get_nonterminal_ptr());
                   });
  if (result_grammar_it == production.get_body().end()) {
    throw exception::unexisted_grammar_symbol_attribute(
        result_attribute_name.get_name());
  }

  size_t terminal_cnt = std::count_if(
      production.get_body().begin(), production.get_body().end(),
      [](auto &grammar_symbol) { return grammar_symbol.is_terminal(); });

  for (auto const &argument : rule.arguments) {
    auto terminal_index_opt = argument.get_terminal_index();
    if (terminal_index_opt) {
      if (terminal_index_opt.value() > terminal_cnt) {
        throw exception::unexisted_grammar_symbol_attribute(
            argument.get_name());
      }
      continue;
    }

    if (argument.belong_to_nonterminal(production.get_head())) {
      inherited_attribute_head_dependency[result_attribute_name].insert(
          argument);
      continue;
    }

    if (!std::any_of(production.get_body().begin(), production.get_body().end(),
                     [&argument](auto &grammar_symbol) {
                       return grammar_symbol.is_nonterminal() &&
                              argument.belong_to_nonterminal(
                                  *grammar_symbol.get_nonterminal_ptr());
                     })) {
      throw exception::unexisted_grammar_symbol_attribute(argument.get_name());
    }
    attribute_dependency[result_attribute_name].insert(argument);
  }

  inherited_attributes.insert(result_attribute_name);
  all_rules[production].emplace_back(std::move(rule));
}

void SDD::check_semantic_rule(const CFG_production &production,
                              const semantic_rule &rule) const {
  if (rule.result_attribute) {
    auto it = all_rules.find(production);
    if (it != all_rules.end()) {
      if (std::any_of(it->second.begin(), it->second.end(), [&rule](auto &r) {
            return r.result_attribute == rule.result_attribute;
          })) {
        throw exception::semantic_rule_confliction(production.get_head());
      }
    }
  }
}

} // namespace cyy::compiler
