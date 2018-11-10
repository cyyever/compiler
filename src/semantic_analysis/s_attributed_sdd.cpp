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

void S_attributed_SDD::run(token_string_view view) {
  check_dependency();

  symbol_string token_names;
  for (auto const &token : view) {
    token_names.push_back(token.name);
  }

  std::map<std::string, std::any>
      all_attributes;

  dynamic_cast<const LR_grammar &>(cfg).parse(
      token_names, [&all_attributes, &view, this](auto const &production) {
        auto it = all_rules.find(production);
        if (it == all_rules.end()) {
          return;
        }

        // right most derivation
        auto const &body = production.second;
        size_t terminal_count =
            std::count_if(body.begin(), body.end(), [](auto grammal_symbol) {
              return grammal_symbol.is_terminal();
            });

        const token_string_view production_view(
            view.data() + view.size() - terminal_count, terminal_count);

        view.remove_suffix(terminal_count);

        auto const &rules = it->second;
        for (auto const &rule : rules) {
          std::vector<std::reference_wrapper<const std::any>>
              argument_values;
          std::vector<std::any> temp_arguments;
          for (auto const &argument : rule.arguments) {
            auto terminal_index =get_terminal_index(argument);
            if (terminal_index) {
              temp_arguments.emplace_back(
                  production_view[terminal_index.value()]);
              argument_values.emplace_back(temp_arguments.back());
            } else {
              argument_values.emplace_back(all_attributes[argument]);
            }
          }
          assert(argument_values.size() == rule.arguments.size());
          rule.action(all_attributes[rule.result_attribute], argument_values);
        }
      });
}

void S_attributed_SDD::check_dependency() const {
  std::set<std::string> passed_attributes;
  auto attribute_dependency = get_attribute_dependency();

  const auto check_attribute_dependency =
      [&passed_attributes, &attribute_dependency](
          auto &&self, const std::string &attribute) {
        const bool is_nonterminal_attribute =
            (attribute.find_first_of('.') != std::string::npos);
        if (!is_nonterminal_attribute) {
          return true;
        }
        if (passed_attributes.count(attribute)) {
          return true;
        }
        auto it = attribute_dependency.find(attribute);
        if (it != attribute_dependency.end()) {
          return false;
        }
        for (const auto &dependent_attribute : it->second) {
          if (!self(self, dependent_attribute)) {
            return false;
          }
        }
        passed_attributes.insert(attribute);
        return true;
      };

  for (const auto &[attribute, _] : attribute_dependency) {
    if (!check_attribute_dependency(check_attribute_dependency, attribute)) {
      throw cyy::compiler::exception::orphan_grammar_symbol_attribute(
          attribute);
    }
  }
}

} // namespace cyy::compiler
