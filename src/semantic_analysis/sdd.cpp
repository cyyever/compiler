/*!
 * \file sdd.cpp
 *
 * \author cyy
 * \date 2018-03-04
 */

#include "sdd.hpp"
#include "../exception.hpp"

namespace std {

template <> struct less<cyy::compiler::SDD::semantic_rule> {
  bool operator()(const cyy::compiler::SDD::semantic_rule &lhs,
                  const cyy::compiler::SDD::semantic_rule &rhs) const noexcept {
    return lhs.attribute < rhs.attribute;
  }
};

} // namespace std

namespace cyy::compiler {

void SDD::add_synthesized_attribute(const CFG::production_type &production,
                                    semantic_rule rule) {
  if (!cfg->has_production(production)) {
    throw exception::unexisted_production("");
  }

  auto it = all_rules.find(production);
  if (it != all_rules.end()) {
    if (it->second.count(rule)) {
      throw exception::semantic_rule_confliction(rule.attribute);
    }
  }

  if (is_attribute_of_nonterminal(production.first, rule.attribute)) {
    throw exception::unexisted_grammar_symbol_attribute(rule.attribute);
  }

  size_t terminal_cnt = std::count_if(
      production.second.begin(), production.second.end(),
      [](auto &grammar_symbol) { return grammar_symbol.is_terminal(); });

  for (auto const &argument : rule.arguments) {
    bool is_nonterminal_attribute =
        (argument.find_first_of('.') != std::string::npos);
    if (!is_nonterminal_attribute) {
      size_t nonterminal_position = 0;
      if (argument.size() > 1 && argument[0] == '$') {
        for (size_t i = 1; i < argument.size(); i++) {
          if (argument[i] < '0' || argument[i] > '9') {
            throw exception::unexisted_grammar_symbol_attribute(rule.attribute);
          }
          nonterminal_position = nonterminal_position * 10 + argument[i] - '0';
        }
      }
      if (nonterminal_position == 0 || nonterminal_position > terminal_cnt) {
        throw exception::unexisted_grammar_symbol_attribute(rule.attribute);
      }
      continue;
    }

    if (!std::any_of(production.second.begin(), production.second.end(),
                     [&argument](auto &grammar_symbol) {
                       return grammar_symbol.is_nonterminal() &&
                              is_attribute_of_nonterminal(
                                  *grammar_symbol.get_nonterminal_ptr(),
                                  argument);
                     })) {

      throw exception::unexisted_grammar_symbol_attribute(argument);
    }
  }
  all_rules[production].emplace(std::move(rule));
}

std::map<SDD::attribute_name_type, std::vector<SDD::attribute_name_type>>
SDD::get_attribute_dependency() const {
  std::map<SDD::attribute_name_type, std::vector<SDD::attribute_name_type>>
      dependency;
  for (auto const &[_, rules] : all_rules) {
    for (auto const &rule : rules) {
      dependency[rule.attribute] = rule.arguments;
    }
  }
  return dependency;
}

bool SDD::is_attribute_of_nonterminal(
    const grammar_symbol_type::nonterminal_type &nonterminal,
    const attribute_name_type &attribute_name) {

  if (attribute_name.size() <= nonterminal.size()) {
    return false;
  }
  const auto pos = attribute_name.find_first_of(nonterminal);

  return pos == 0 && attribute_name[nonterminal.size()] == '.';
}
} // namespace cyy::compiler
