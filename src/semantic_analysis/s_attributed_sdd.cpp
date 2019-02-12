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
#include "s_attributed_sdd.hpp"

namespace cyy::compiler {

std::map<grammar_symbol_attribute_name, std::any>
S_attributed_SDD::run(token_span span) {
  check_attributes();
  if (span.empty()) {
    std::cerr << "span is empty" << std::endl;
    return {};
  }

  symbol_string token_names;
  for (auto const &token : span) {
    token_names.push_back(token.name);
  }

  std::map<grammar_symbol_attribute_name, std::any> all_attributes;
  std::vector<size_t> terminal_positions;
  size_t next_position = 0;
  dynamic_cast<const LR_grammar &>(cfg).parse(
      token_names,
      [&terminal_positions, &next_position]([[maybe_unused]] auto terminal) {
        terminal_positions.push_back(next_position);
        next_position++;
      },
      [&all_attributes, &span, this, &terminal_positions](auto const &production ) {
      auto const &head=production.get_head();
      auto const &body=production.get_body();

        auto it = all_rules.find(production);
        //{head, body});
        if (it == all_rules.end()) {
          return;
        }

        auto const terminal_count = std::count_if(
            body.begin(), body.end(), [this](auto const &grammal_symbol) {
              return grammal_symbol.is_terminal() &&
                     !grammal_symbol.is_epsilon(cfg.get_alphabet());
            });

        const auto token_position_span =
            gsl::span<size_t>(terminal_positions).last(terminal_count);
        auto const &rules = it->second;
        for (auto const &rule : rules) {
          std::vector<std::reference_wrapper<const std::any>> argument_values;
          std::vector<std::any> token_vector;
          for (auto const &argument : rule.arguments) {
            auto terminal_index = argument.get_terminal_index();
            if (terminal_index) {
              token_vector.emplace_back(
                  span.at(token_position_span.at(terminal_index.value())));
              argument_values.emplace_back(token_vector.back());
            } else {
              if (!all_attributes.count(argument)) {
                throw cyy::compiler::exception::orphan_grammar_symbol_attribute(
                    argument.get_name());
              }
              argument_values.emplace_back(all_attributes[argument]);
            }
          }
          assert(argument_values.size() == rule.arguments.size());
          auto result_attribute_opt = rule.action(argument_values);
          if (rule.result_attribute) {
            all_attributes[rule.result_attribute.value()] =
                std::move(result_attribute_opt.value());
          }
        }
        terminal_positions.resize(terminal_positions.size() - terminal_count);
      });
  return all_attributes;
}

void S_attributed_SDD::check_attributes() const {
  for (auto const &attribute : synthesized_attributes) {
    auto it = synthesized_attribute_dependency.find(attribute);
    if (it == synthesized_attribute_dependency.end() || it->second.empty()) {
      continue;
    }
    if (!std::includes(synthesized_attributes.begin(),
                       synthesized_attributes.end(), it->second.begin(),
                       it->second.end(),
                       std::less<grammar_symbol_attribute_name>())) {
      throw exception::no_synthesized_grammar_symbol_attribute(
          attribute.get_name());
    }
  }
}
} // namespace cyy::compiler
