/*!
 * \file sdd.cpp
 *
 * \author cyy
 * \date 2018-03-04
 */

#include <cassert>
#include <cyy/computation/util.hpp>

#include "../exception.hpp"
#include "sdd.hpp"

namespace cyy::compiler {

  void SDD::add_synthesized_attribute(const CFG_production &production,
                                      semantic_rule rule) {
    check_semantic_rule(production, rule);

    if (rule.result_attribute) {
      const auto &result_attribute_name = rule.result_attribute.value();
      if (result_attribute_name.get_index() != 0) {
        throw exception::no_synthesized_grammar_symbol_attribute(
            result_attribute_name.get_name());
      }
      synthesized_attributes.insert(
          rule.result_attribute->get_full_name(production));
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

    inherited_attributes.insert(
        rule.result_attribute->get_full_name(production));
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
      if (!argument.belong_to_production(production)) {
        throw exception::unexisted_grammar_symbol_attribute(
            argument.get_name());
      }
    }
  }

  void SDD::resolve_semantic_rules_order() const {
    for (auto &[_, rules] : all_rules) {
      assert(!rules.empty());
      std::map<std::string, size_t> result_attribute_rule_indexes;
      for (size_t i = 0; i < rules.size(); i++) {
        auto const &rule = rules[i];
        if (rule.result_attribute) {
          auto const &result_attribute_name = rule.result_attribute.value();
          result_attribute_rule_indexes[result_attribute_name.get_name()] = i;
        }
      }

      std::map<size_t, std::set<size_t>> dependency_graph;
      for (size_t i = 0; i < rules.size(); i++) {
        auto const &rule = rules[i];
        for (auto const &argument : rule.arguments) {
          if (!rule.result_attribute && argument.get_index() != 0) {
            continue;
          }
          if (rule.result_attribute &&
              argument.get_index() != rule.result_attribute->get_index()) {
            continue;
          }

          auto it = result_attribute_rule_indexes.find(argument.get_name());
          if (it == result_attribute_rule_indexes.end()) {
            continue;
          }
          if (it->second == i) {
            throw exception::grammar_symbol_attribute_dependency_circle(
                argument.get_name());
          }
          dependency_graph[it->second].insert(i);
        }
      }
      if (dependency_graph.empty()) {
        continue;
      }
      auto [sorted_indexes, remain_dependency] =
          topological_sort(dependency_graph);
      if (!remain_dependency.empty()) {
        throw exception::grammar_symbol_attribute_dependency_circle(
            rules[remain_dependency.begin()->first]
                .result_attribute->get_name());
      }

      for (auto it = sorted_indexes.begin(); it != sorted_indexes.end(); it++) {
        auto it2 = std::min_element(it, sorted_indexes.end());
        if (it == it2) {
          continue;
        }
        std::swap(rules[*it], rules[*it2]);
        *it2 = *it;
      }
    }
  }

} // namespace cyy::compiler
