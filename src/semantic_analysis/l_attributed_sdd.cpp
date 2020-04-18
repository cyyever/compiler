/*!
 * \file s_attributed_sdd.cpp
 *
 * \brief
 * \author cyy
 * \date 2018-10-28
 */
#include <cassert>
#include <functional>
#include <range/v3/algorithm.hpp>
#include <set>

#include "../exception.hpp"
#include "l_attributed_sdd.hpp"

namespace cyy::compiler {

  std::optional<std::map<std::string, std::any>> L_attributed_SDD::_run(
      token_span span,
      const std::unordered_set<std::string> &result_attribute_names) const {
    if (new_rule_flag) {
      check_attributes();
      resolve_semantic_rules_order();
      new_rule_flag = false;
    }

    symbol_string token_names;
    for (auto const &token : span) {
      token_names.push_back(token.name);
    }

    std::vector<std::map<std::string, std::any>>
        grammal_symbol_attributes_stack;
    // for start symbol
    grammal_symbol_attributes_stack.emplace_back();
    size_t next_position = 0;

    std::map<std::string, std::any> final_attributes;
    if (!dynamic_cast<const LL_grammar &>(cfg).parse(
            token_names,
            [this, &span, &result_attribute_names, &final_attributes,
             &next_position, &grammal_symbol_attributes_stack](
                const auto &production, auto pos) {
              const auto &body = production.get_body();
              const auto body_size = production.get_body().size();
              // after terminal matching
              if (pos > 0 && body[pos - 1].is_terminal()) {
                grammal_symbol_attributes_stack.emplace_back();
                assert(next_position < static_cast<size_t>(span.size()));
                grammal_symbol_attributes_stack.back().emplace(
                    "token", span[next_position]);
                next_position++;
              }

              const auto rules_it = all_rules.find(production);

              // before nonterminal derivation
              if (pos < body_size && body[pos].is_nonterminal()) {
                grammal_symbol_attributes_stack.emplace_back();
                if (rules_it != all_rules.end()) {
                  auto stack_size = grammal_symbol_attributes_stack.size();
                  // compute inherited attributes defined without their
                  // synthesized attributes
                  for (auto const &rule : rules_it->second) {
                    if (!rule.result_attribute) {
                      continue;
                    }

                    auto result_attribute_index =
                        rule.result_attribute->get_index();
                    if (result_attribute_index != pos + 1) {
                      continue;
                    }

                    if (ranges::any_of(
                            rule.arguments,
                            [this, &production,
                             result_attribute_index](auto const &argument) {
                              return argument.get_index() ==
                                         result_attribute_index &&
                                     synthesized_attributes.count(
                                         argument.get_full_name(production)) !=
                                         0;
                            })) {
                      continue;
                    }

                    std::vector<const std::any *> argument_values;
                    for (auto const &argument : rule.arguments) {
                      auto index = argument.get_index();

                      auto &grammar_symbol_attributes =
                          grammal_symbol_attributes_stack
                              [stack_size - 1 - result_attribute_index + index];
                      auto it2 = grammar_symbol_attributes.find(
                          argument.get_full_name(production));
                      if (it2 == grammar_symbol_attributes.end()) {
                        throw exception::unexisted_grammar_symbol_attribute(
                            argument.get_name());
                      }
                      argument_values.emplace_back(&(it2->second));
                    }

                    auto result_value_opt = rule.action(argument_values);
                    assert(rule.result_attribute);
                    if (!result_value_opt) {
                      throw exception::unexisted_grammar_symbol_attribute(
                          rule.result_attribute->get_name());
                    }

                    auto attribute_full_name =
                        rule.result_attribute.value().get_full_name(production);
                    if (result_attribute_names.count(attribute_full_name) !=
                        0) {
                      final_attributes.insert_or_assign(
                          attribute_full_name, result_value_opt.value());
                    }
                    grammal_symbol_attributes_stack
                        .back()[attribute_full_name] =
                        std::move(result_value_opt.value());
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

                  auto result_attribute_index =
                      rule.result_attribute->get_index();
                  if (result_attribute_index != pos) {
                    continue;
                  }

                  if (!ranges::any_of(
                          rule.arguments,
                          [this, &production,
                           result_attribute_index](auto const &argument) {
                            return argument.get_index() ==
                                       result_attribute_index &&
                                   synthesized_attributes.count(
                                       argument.get_full_name(production)) != 0;
                          })) {
                    continue;
                  }

                  std::vector<const std::any *> argument_values;
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
                    argument_values.emplace_back(&(it2->second));
                  }

                  auto result_value_opt = rule.action(argument_values);
                  assert(rule.result_attribute);
                  if (!result_value_opt) {
                    throw exception::unexisted_grammar_symbol_attribute(
                        rule.result_attribute->get_name());
                  }
                  auto attribute_full_name =
                      rule.result_attribute.value().get_full_name(production);
                  if (result_attribute_names.count(attribute_full_name) != 0) {
                    final_attributes.insert_or_assign(attribute_full_name,
                                                      result_value_opt.value());
                  }
                  grammal_symbol_attributes_stack.back()[attribute_full_name] =
                      std::move(result_value_opt.value());
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

                    std::vector<const std::any *> argument_values;
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
                      argument_values.emplace_back(&(it2->second));
                    }

                    auto result_value_opt = rule.action(argument_values);
                    assert(rule.result_attribute);
                    if (!result_value_opt) {
                      throw exception::unexisted_grammar_symbol_attribute(
                          rule.result_attribute->get_name());
                    }
                    auto attribute_full_name =
                        rule.result_attribute.value().get_full_name(production);
                    if (result_attribute_names.count(attribute_full_name) !=
                        0) {
                      final_attributes.insert_or_assign(
                          attribute_full_name, result_value_opt.value());
                    }
                    grammal_symbol_attributes_stack[stack_size - 1 - body_size]
                                                   [attribute_full_name] =
                                                       std::move(
                                                           result_value_opt
                                                               .value());
                  }
                }
                grammal_symbol_attributes_stack.resize(stack_size - body_size);
              }
            })) {
      std::cerr << "failed to parse span" << std::endl;
      return {};
    }
    assert(grammal_symbol_attributes_stack.size() == 1);
    return final_attributes;
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
          const auto index = argument.get_index();
          if (index > result_attribute_index) {
            throw exception::no_inherited_grammar_symbol_attribute(
                rule.result_attribute->get_name());
          }

          // head's attribute must be inherited
          if (index == 0) {
            if (!inherited_attributes.count(
                    argument.get_full_name(production))) {
              throw exception::no_inherited_grammar_symbol_attribute(
                  argument.get_name());
            }
          }
        }
      }
    }
  }
} // namespace cyy::compiler
