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

#include "../exception.hpp"
#include "l_attributed_sdd.hpp"

namespace cyy::compiler {

  std::map<std::string, std::any> L_attributed_SDD::run(token_span span) {
    if (new_rule_flag) {
      check_attributes();
      resolve_semantic_rules_order();
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
    // for start symbol
    grammal_symbol_attributes_stack.emplace_back();
    std::vector<size_t> terminal_positions;
    size_t next_position = 0;

    dynamic_cast<const LL_grammar &>(cfg).parse(
        token_names,
        [this, &span, &next_position,
         &grammal_symbol_attributes_stack](const auto &production, auto pos) {
          const auto &body = production.get_body();
          const auto body_size = production.get_body().size();
          // after terminal matching
          if (pos > 0 && body[pos - 1].is_terminal()) {
            grammal_symbol_attributes_stack.emplace_back();
            auto const terminal = *body[pos - 1].get_terminal_ptr();
            if (!cfg.get_alphabet().is_epsilon(terminal)) {
              assert(next_position < static_cast<size_t>(span.size()));
              grammal_symbol_attributes_stack.back().emplace(
                  "token", span[next_position]);
              next_position++;
            }
          }

          const auto rules_it = all_rules.find(production);

          // before nonterminal derivation
          if (pos < body_size && body[pos].is_nonterminal()) {
            grammal_symbol_attributes_stack.emplace_back();
            if (rules_it != all_rules.end()) {
              auto stack_size = grammal_symbol_attributes_stack.size();
              // compute inherited attributes defined without their synthesized
              // attributes
              for (auto const &rule : rules_it->second) {
                if (!rule.result_attribute) {
                  continue;
                }

                auto result_attribute_index =
                    rule.result_attribute->get_index();
                if (result_attribute_index != pos + 1) {
                  continue;
                }

                if (std::any_of(rule.arguments.begin(), rule.arguments.end(),
                                [this, &production,
                                 result_attribute_index](auto const &argument) {
                                  return argument.get_index() ==
                                             result_attribute_index &&
                                         synthesized_attributes.count(
                                             argument.get_full_name(
                                                 production)) != 0;
                                })) {
                  continue;
                }

                std::vector<std::reference_wrapper<const std::any>>
                    argument_values;
                for (auto const &argument : rule.arguments) {
                  auto index = argument.get_index();

                  auto &grammar_symbol_attributes =
                      grammal_symbol_attributes_stack[stack_size - 1 -
                                                      result_attribute_index +
                                                      index];
                  auto it2 = grammar_symbol_attributes.find(
                      argument.get_full_name(production));
                  if (it2 == grammar_symbol_attributes.end()) {
                    throw exception::unexisted_grammar_symbol_attribute(
                        argument.get_name());
                  }
                  argument_values.emplace_back(it2->second);
                }

                auto result_value_opt = rule.action(argument_values);
                assert(rule.result_attribute);
                if (!result_value_opt) {
                  throw exception::unexisted_grammar_symbol_attribute(
                      rule.result_attribute->get_name());
                }
                grammal_symbol_attributes_stack
                    .back()[rule.result_attribute.value().get_full_name(
                        production)] = std::move(result_value_opt.value());
              }
            }
          }

          // after nonterminal derivation
          if (pos > 0 && body[pos - 1].is_nonterminal() &&
              rules_it != all_rules.end()) {
            auto stack_size = grammal_symbol_attributes_stack.size();
            // compute inherited attributes defined with their synthesized
            // attributes
            for (auto const &rule : rules_it->second) {
              if (!rule.result_attribute) {
                continue;
              }

              auto result_attribute_index = rule.result_attribute->get_index();
              if (result_attribute_index != pos) {
                continue;
              }

              if (!std::any_of(
                      rule.arguments.begin(), rule.arguments.end(),
                      [this, &production,
                       result_attribute_index](auto const &argument) {
                        return argument.get_index() == result_attribute_index &&
                               synthesized_attributes.count(
                                   argument.get_full_name(production)) != 0;
                      })) {
                continue;
              }

              std::vector<std::reference_wrapper<const std::any>>
                  argument_values;
              for (auto const &argument : rule.arguments) {
                auto index = argument.get_index();
                auto &grammar_symbol_attributes =
                    grammal_symbol_attributes_stack[stack_size - 1 -
                                                    result_attribute_index +
                                                    index];
                auto it2 = grammar_symbol_attributes.find(
                    argument.get_full_name(production));
                if (it2 == grammar_symbol_attributes.end()) {
                  throw exception::unexisted_grammar_symbol_attribute(
                      argument.get_name());
                }
                argument_values.emplace_back(it2->second);
              }

              auto result_value_opt = rule.action(argument_values);
              assert(rule.result_attribute);
              if (!result_value_opt) {
                throw exception::unexisted_grammar_symbol_attribute(
                    rule.result_attribute->get_name());
              }
              grammal_symbol_attributes_stack
                  .back()[rule.result_attribute.value().get_full_name(
                      production)] = std::move(result_value_opt.value());
            }
          }

          // finish nonterminal derivation
          if (pos == body_size) {

            auto stack_size = grammal_symbol_attributes_stack.size();
            if (rules_it != all_rules.end()) {
              // compute synthesized attributes
              for (auto const &rule : rules_it->second) {
                if (rule.result_attribute &&
                    rule.result_attribute->get_index() != 0) {
                  continue;
                }

                std::vector<std::reference_wrapper<const std::any>>
                    argument_values;
                for (auto const &argument : rule.arguments) {
                  auto index = argument.get_index();

                  auto &grammar_symbol_attributes =
                      grammal_symbol_attributes_stack[stack_size - 1 -
                                                      body_size + index];

                  auto it2 = grammar_symbol_attributes.find(
                      argument.get_full_name(production));
                  if (it2 == grammar_symbol_attributes.end()) {
                    throw exception::unexisted_grammar_symbol_attribute(
                        argument.get_full_name(production));
                  }
                  argument_values.emplace_back(it2->second);
                }

                auto result_value_opt = rule.action(argument_values);
                assert(rule.result_attribute);
                if (!result_value_opt) {
                  throw exception::unexisted_grammar_symbol_attribute(
                      rule.result_attribute->get_name());
                }
                grammal_symbol_attributes_stack
                    [stack_size - 1 - body_size]
                    [rule.result_attribute.value().get_full_name(production)] =
                        std::move(result_value_opt.value());
              }
            }
            grammal_symbol_attributes_stack.resize(stack_size - body_size);
          }
        });
    assert(grammal_symbol_attributes_stack.size() == 1);
    return grammal_symbol_attributes_stack[0];
    return {};
  }

  void L_attributed_SDD::check_attributes() const {
    for (const auto &[production, rules] : all_rules) {
      assert(!rules.empty());
      for (auto const &rule : rules) {
        // synthesized attribute
        if (!rule.result_attribute) {
          continue;
        }
        auto result_attribute_index = rule.result_attribute->get_index();
        // synthesized attribute
        if (result_attribute_index == 0) {
          continue;
        }

        for (auto const &argument : rule.arguments) {
          auto index = argument.get_index();
          if (index > result_attribute_index) {
            throw exception::no_inherited_grammar_symbol_attribute(
                rule.result_attribute->get_name());
          }

          // head's attribute must be inherited
          if (index == 0) {
            if (!inherited_attributes.count(
                    argument.get_full_name(production))) {
              throw exception::no_synthesized_grammar_symbol_attribute(
                  argument.get_name());
            }
          }
        }
      }
    }
  }
} // namespace cyy::compiler
