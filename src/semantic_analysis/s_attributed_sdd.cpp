/*!
 * \file s_attributed_sdd.cpp
 *
 * \brief
 * \author cyy
 * \date 2018-10-28
 */
#include <algorithm>
#include <cassert>
#include <functional>
#include <set>

#include <cyy/computation/util.hpp>

#include "../exception.hpp"
#include "s_attributed_sdd.hpp"

namespace cyy::compiler {

  std::map<std::string, std::any> S_attributed_SDD::run(token_span span) {
    if (new_rule_flag) {
      check_attribute_dependency();
      new_rule_flag = false;
    }
    if (span.empty()) {
      std::cerr << "span is empty" << std::endl;
      return {};
    }

    symbol_string token_names;
    for (auto const &token : span) {
      token_names.push_back(token.name);
    }

    std::vector<std::map<std::string, std::any>>
        grammal_symbol_attributes_stack;
    std::vector<size_t> terminal_positions;
    size_t next_position = 0;
    dynamic_cast<const LR_grammar &>(cfg).parse(
        token_names,
        [this, &span, &next_position,
         &grammal_symbol_attributes_stack]([[maybe_unused]] auto terminal) {
          if (cfg.get_alphabet().is_epsilon(terminal)) {
            grammal_symbol_attributes_stack.emplace_back();
            return;
          }
          assert(next_position < static_cast<size_t>(span.size()));
          grammal_symbol_attributes_stack.emplace_back();
          grammal_symbol_attributes_stack.back().emplace("token",
                                                         span[next_position]);
          next_position++;
        },
        [&span, &grammal_symbol_attributes_stack,
         this](auto const &production) {
          auto it = all_rules.find(production);
          if (it == all_rules.end()) {
            return;
          }
          const auto body_size = production.get_body().size();
          const auto stake_size = grammal_symbol_attributes_stack.size();

          std::map<std::string, std::any> result_attributes;

          for (auto const &rule : it->second) {
            std::vector<std::reference_wrapper<const std::any>> argument_values;
            for (auto const &argument : rule.arguments) {
              auto index = argument.get_index();
              auto &grammar_symbol_attributes =
                  index == 0
                      ? result_attributes
                      : grammal_symbol_attributes_stack[stake_size - body_size -
                                                        1 + index];

              auto it2 = grammar_symbol_attributes.find(
                  argument.get_full_name(production));
              if (it2 == grammar_symbol_attributes.end()) {
                throw exception::unexisted_grammar_symbol_attribute(
                    argument.get_name());
              }
              argument_values.emplace_back(it2->second);
            }
            auto result_attribute_opt = rule.action(argument_values);
            if (rule.result_attribute) {
              assert(result_attribute_opt);
              result_attributes[rule.result_attribute.value().get_full_name(
                  production)] = std::move(result_attribute_opt.value());
            } else {
              assert(!result_attribute_opt);
            }
          }
          grammal_symbol_attributes_stack.resize(stake_size - body_size);
          grammal_symbol_attributes_stack.emplace_back(
              std::move(result_attributes));
        });
    assert(grammal_symbol_attributes_stack.size() == 1);
    return grammal_symbol_attributes_stack[0];
  }

  void S_attributed_SDD::check_attribute_dependency() {
    for (auto &[_, rules] : all_rules) {
      assert(!rules.empty());
      std::map<std::string, size_t> result_attributes;
      for (size_t i = 0; i < rules.size(); i++) {
        auto const &rule = rules[i];
        if (rule.result_attribute) {
          result_attributes[rule.result_attribute->get_suffix()] = i;
        }
      }
      std::map<size_t, std::set<size_t>> dependency_graph;
      for (size_t i = 0; i < rules.size(); i++) {
        auto const &rule = rules[i];
        for (auto const &argument : rule.arguments) {
          if (argument.get_index() == 0) {
            auto it = result_attributes.find(argument.get_suffix());
            if (it == result_attributes.end()) {
              throw exception::unexisted_grammar_symbol_attribute(
                  argument.get_name());
            }
            if (it->second == i) {
              throw exception::grammar_symbol_attribute_dependency_circle(
                  argument.get_name());
            }
            dependency_graph[it->second].insert(i);
          }
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
