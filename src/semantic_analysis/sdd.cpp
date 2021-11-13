/*!
 * \file sdd.cpp
 *
 * \author cyy
 * \date 2018-03-04
 */

#include "sdd.hpp"

#include <algorithm>
#include <cassert>
#include <ranges>

#include <cyy/algorithm/graph/dag.hpp>

#include "exception.hpp"

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
    /*
    if (!rule.result_attribute) {
      throw exception::no_inherited_grammar_symbol_attribute(
          "no result_attribute");
    }
    */

    if (rule.result_attribute) {
      const auto &result_attribute_name = rule.result_attribute.value();
      if (result_attribute_name.get_index() == 0) {
        throw exception::no_inherited_grammar_symbol_attribute(
            result_attribute_name.get_name());
      }

      inherited_attributes.insert(
          rule.result_attribute->get_full_name(production));
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
        if (std::ranges::any_of(it->second, [&rule](auto &r) {
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
      std::unordered_map<std::string, size_t> result_attribute_rule_indexes;
      for (size_t i = 0; i < rules.size(); i++) {
        auto const &rule = rules[i];
        if (rule.result_attribute) {
          auto const &result_attribute_name = rule.result_attribute.value();
          result_attribute_rule_indexes[result_attribute_name.get_name()] = i;
        }
      }

      cyy::algorithm::DAG<size_t, size_t> dependency_dag;
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
          dependency_dag.add_edge({it->second, i});
        }
      }
      if (dependency_dag.get_edge_number() == 0) {
        continue;
      }
      auto sorted_indexes_opt = dependency_dag.get_topological_ordering();
      if (!sorted_indexes_opt.has_value()) {
        throw exception::grammar_symbol_attribute_dependency_circle("");
      }
      auto sorted_rules = std::move(rules);
      rules.clear();
      for (auto idx : sorted_indexes_opt.value()) {
        rules.emplace_back(std::move(sorted_rules[idx]));
      }
    }
  }

  std::optional<std::unordered_map<std::string, std::any>> SDD::run(
      token_span span,
      const std::unordered_set<std::string> &result_attribute_names) const {
    if (span.empty()) {
      std::cerr << "span is empty" << std::endl;
      return {};
    }

    auto result_attribute_opt = _run(span, result_attribute_names);
    if (!result_attribute_opt.has_value()) {
      std::cerr << "failed to run sdd" << std::endl;
      return {};
    }
    if (!result_attribute_names.empty()) {
      if (result_attribute_opt.value().size() !=
          result_attribute_names.size()) {
        for (auto const &name : result_attribute_names) {
          if (result_attribute_opt.value().count(name) == 0) {
            std::cerr << "no result attribute " << name << std::endl;
          }
        }
        return {};
      }
    }
    return result_attribute_opt;
  }
} // namespace cyy::compiler
