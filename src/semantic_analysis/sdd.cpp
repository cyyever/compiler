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
    check_semantic_rule(production, rule);

    if (!rule.result_attribute) {
      throw exception::no_synthesized_grammar_symbol_attribute(
          "no result_attribute");
    }

    const auto &result_attribute_name = rule.result_attribute.value();
    if (result_attribute_name.get_index() != 0) {
      throw exception::no_synthesized_grammar_symbol_attribute(
          result_attribute_name.get_name());
    }

    all_rules[production].emplace_back(std::move(rule));
    new_rule_flag = true;
  }

  void SDD::add_inherited_attribute(const CFG_production &production,
                                    semantic_rule rule) {
    check_semantic_rule(production, rule);
    if (!rule.result_attribute) {
      throw exception::no_inherited_grammar_symbol_attribute(
          "no result_attribute");
    }

    const auto &result_attribute_name = rule.result_attribute.value();
    if (result_attribute_name.get_index() == 0) {
      throw exception::no_inherited_grammar_symbol_attribute(
          result_attribute_name.get_name());
    }

    all_rules[production].emplace_back(std::move(rule));
    new_rule_flag = true;
  }

  void SDD::check_semantic_rule(const CFG_production &production,
                                const semantic_rule &rule) const {
    if (!cfg.has_production(production)) {
      throw exception::unexisted_production("");
    }
    if (rule.result_attribute) {
      auto it = all_rules.find(production);
      if (it != all_rules.end()) {
        if (std::any_of(it->second.begin(), it->second.end(), [&rule](auto &r) {
              return r.result_attribute == rule.result_attribute;
            })) {
          throw exception::semantic_rule_confliction(production.get_head());
        }
      }

      const auto &result_attribute_name = rule.result_attribute.value();
      if (!result_attribute_name.belong_to_nonterminal()) {
        throw exception::invalid_semantic_rule(
            result_attribute_name.get_name());
      }
    }

    for (auto const &argument : rule.arguments) {
      const auto index = argument.get_index();
      if (index == 0) {
        if (!argument.match(production.get_head())) {
          throw exception::unexisted_grammar_symbol_attribute(
              argument.get_name());
        }
        continue;
      }
      if (index > production.get_body().size()) {
        throw exception::unexisted_grammar_symbol_attribute(
            argument.get_name());
      }

      auto const &grammar_symbol =
          production.get_body()[argument.get_index() - 1];
      if (!argument.match(grammar_symbol)) {
        throw exception::unexisted_grammar_symbol_attribute(
            argument.get_name());
      }
    }
  }

} // namespace cyy::compiler
