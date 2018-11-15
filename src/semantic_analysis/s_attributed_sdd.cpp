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

std::map<std::string, std::any> S_attributed_SDD::run(token_span span) {
  if (span.empty()) {
    std::cerr << "span is empty" << std::endl;
    return {};
  }

  symbol_string token_names;
  for (auto const &token : span) {
    token_names.push_back(token.name);
  }

  std::map<std::string, std::any> all_attributes;

  dynamic_cast<const LR_grammar &>(cfg).parse(
      token_names, []([[maybe_unused]] auto terminal) {},
      [&all_attributes, &span, this](auto const &head, const auto &body,
                                     auto token_position_span) {
        auto it = all_rules.find({head, body});
        if (it == all_rules.end()) {
          return;
        }

        auto const &rules = it->second;
        for (auto const &rule : rules) {
          std::vector<std::reference_wrapper<const std::any>> argument_values;
          std::vector<std::any> token_vector;
          for (auto const &argument : rule.arguments) {
            auto terminal_index = get_terminal_index(argument);
            if (terminal_index) {
              token_vector.emplace_back(
                  span.at(token_position_span.at(terminal_index.value())));
              argument_values.emplace_back(token_vector.back());
            } else {
              if (!all_attributes.count(argument)) {
                throw cyy::compiler::exception::orphan_grammar_symbol_attribute(
                    argument);
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
      });
  return all_attributes;
}

} // namespace cyy::compiler
