/*!
 * \file sdd.cpp
 *
 * \author cyy
 * \date 2018-03-04
 */

#include "sdd.hpp"
#include "../exception.hpp"

namespace std {

/*
template <> struct less<cyy::compiler::SDD::attribute_name_type> {
  bool operator()(const cyy::compiler::SDD::attribute_name_type &lhs,
                  const cyy::compiler::SDD::attribute_name_type &rhs) const
noexcept { return lhs.get_name() < rhs.get_name();
  }
};
*/

} // namespace std

namespace cyy::compiler {

void SDD::add_synthesized_attribute(const CFG::production_type &production,
                                    semantic_rule rule) {
  if (!cfg.has_production(production)) {
    throw exception::unexisted_production("");
  }

  auto it = all_rules.find(production);
  if (it != all_rules.end()) {
    if (it->second.count(rule)) {
      throw exception::semantic_rule_confliction(rule.result_attribute);
    }
  }

  if (!belong_nonterminal(rule.result_attribute, production.first)) {
    throw exception::unexisted_grammar_symbol_attribute(rule.result_attribute);
  }

  size_t terminal_cnt = std::count_if(
      production.second.begin(), production.second.end(),
      [](auto &grammar_symbol) { return grammar_symbol.is_terminal(); });

  for (auto const &argument : rule.arguments) {
    auto terminal_index_opt = get_terminal_index(argument);
    if (terminal_index_opt) {
      if (terminal_index_opt.value() > terminal_cnt) {
        throw exception::unexisted_grammar_symbol_attribute(argument);
      }
      continue;
    }

    if (!std::any_of(production.second.begin(), production.second.end(),
                     [&argument](auto &grammar_symbol) {
                       return grammar_symbol.is_nonterminal() &&
                              belong_nonterminal(
                                  argument,
                                  *grammar_symbol.get_nonterminal_ptr());
                     })) {
      throw exception::unexisted_grammar_symbol_attribute(argument);
    }
  }
  all_rules[production].emplace(std::move(rule));
}

std::map<std::string, std::vector<std::string>>
SDD::get_attribute_dependency() const {
  std::map<std::string, std::vector<std::string>> dependency;
  for (auto const &[_, rules] : all_rules) {
    for (auto const &rule : rules) {
      dependency[rule.result_attribute] = rule.arguments;
    }
  }
  return dependency;
}

bool SDD::belong_nonterminal(
    const std::string &name,
    const grammar_symbol_type::nonterminal_type &nonterminal) {

  if (name.size() <= nonterminal.size()) {
    return false;
  }
  const auto pos = name.find_first_of(nonterminal);

  return pos == 0 && name[nonterminal.size()] == '.';
}

std::optional<size_t> SDD::get_terminal_index(const std::string &name) {

  if (name.size() > 1 && name[0] == '$') {
    size_t index = 0;
    for (size_t i = 1; i < name.size(); i++) {
      if (name[i] < '0' || name[i] > '9') {
        return {};
      }
      index = index * 10 + name[i] - '0';
    }
    return {index};
  }
  return {};
}

} // namespace cyy::compiler
