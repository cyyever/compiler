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

#include "exception.hpp"
#include "s_attributed_sdd.hpp"

namespace cyy::compiler {

  std::optional<std::unordered_map<std::string, std::any>>
  S_attributed_SDD::_run(
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

    std::vector<std::unordered_map<std::string, std::any>>
        grammal_symbol_attributes_stack;
    size_t next_position = 0;
    std::unordered_map<std::string, std::any> final_attributes;
    if (!dynamic_cast<const LR_grammar &>(cfg).parse(
            token_names,
            [&span, &next_position,
             &grammal_symbol_attributes_stack]([[maybe_unused]] auto terminal) {
              assert(next_position < static_cast<size_t>(span.size()));
              grammal_symbol_attributes_stack.emplace_back();
              grammal_symbol_attributes_stack.back().emplace(
                  "token", span[static_cast<std::ptrdiff_t>(next_position)]);
              next_position++;
            },
            [&result_attribute_names, &final_attributes,
             &grammal_symbol_attributes_stack, this](auto const &production) {
              const auto body_size = production.get_body().size();
              const auto stake_size = grammal_symbol_attributes_stack.size();

              std::unordered_map<std::string, std::any> result_attributes;

              auto it = all_rules.find(production);
              if (it != all_rules.end()) {
                for (auto const &rule : it->second) {
                  std::vector<const std::any *> argument_values;
                  for (auto const &argument : rule.arguments) {
                    auto index = argument.get_index();
                    auto &grammar_symbol_attributes =
                        index == 0
                            ? result_attributes
                            : grammal_symbol_attributes_stack[stake_size -
                                                              body_size - 1 +
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
                  if (rule.result_attribute) {
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
                    result_attributes[attribute_full_name] =
                        std::move(result_value_opt.value());
                  }
                }
              }
              grammal_symbol_attributes_stack.resize(stake_size - body_size);
              grammal_symbol_attributes_stack.emplace_back(
                  std::move(result_attributes));
            })) {
      std::cerr << "failed to parse span" << std::endl;
      return {};
    }
    assert(grammal_symbol_attributes_stack.size() == 1);

    return final_attributes;
  }

  void S_attributed_SDD::check_attributes() const {
    for (const auto &[production, rules] : all_rules) {
      for (auto const &rule : rules) {
        // synthesized attribute
        if (rule.result_attribute) {
          auto full_name = rule.result_attribute->get_full_name(production);
          if (!synthesized_attributes.count(full_name)) {
            throw exception::no_synthesized_grammar_symbol_attribute(full_name);
          }
        }
        for (auto const &argument : rule.arguments) {
          if (!argument.belong_to_nonterminal()) {
            continue;
          }
          auto full_name = argument.get_full_name(production);
          if (!synthesized_attributes.count(full_name)) {
            throw exception::no_synthesized_grammar_symbol_attribute(full_name);
          }
        }
      }
    }
  }

} // namespace cyy::compiler
