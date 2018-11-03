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
    return lhs.attribute<rhs.attribute;
  }
};

} // namespace std

namespace cyy::compiler {

void SDD::add_synthesized_attribute(const CFG::production_type &production,
                                    semantic_rule rule) {
  if (!cfg->has_production(production)) {
    throw exception::unexisted_production("");
  }

  auto nonterminal_ptr=rule.attribute.get_nonterminal_ptr();
  if(!nonterminal_ptr) {
    throw exception::invalid_semantic_rule("rule for terminal");
  }
  auto it=all_rules.find(production);
  if(it!=all_rules.end()) {
	  if(it->second.count(rule)) {
		  throw exception::semantic_rule_confliction(*nonterminal_ptr);
	  }
  }

  if (is_attribute_of_grammar_symbol(production.first, rule.attribute)) {
    throw exception::unexisted_grammar_symbol_attribute("");
  }

  for (auto const &argument : rule.arguments) {
    bool flag = false;
    for (auto const &body_grammar_symbol : production.second) {
      if (is_attribute_of_grammar_symbol(body_grammar_symbol, argument)) {
        flag = true;
        break;
      }
    }
    if (!flag && is_attribute_of_grammar_symbol(production.first, argument)) {
      flag = true;
    }
    if (!flag) {
      throw exception::unexisted_grammar_symbol_attribute("");
    }
  }
  all_rules[production].emplace(std::move(rule));
}

std::map<SDD::attribute_name_type,std::vector<SDD::attribute_name_type>> SDD::get_attribute_dependency() const {
	std::map<SDD::attribute_name_type,std::vector<SDD::attribute_name_type>> dependency;
	for(auto const &[_,rules]:all_rules) {
	for(auto const &rule:rules) {
		dependency[rule.attribute]=rule.arguments;
	}
	}
	return dependency;
  }
} // namespace cyy::compiler
