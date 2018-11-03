/*!
 * \file s_attributed_sdd.cpp
 *
 * \brief
 * \author cyy
 * \date 2018-10-28
 */

#include <algorithm>
#include "s_attributed_sdd.hpp"
#include "../exception.hpp"

namespace cyy::compiler {

  void S_attributed_SDD::run(token_string_view view) {
	  check_dependency();

    symbol_string token_names;
    for(auto const &token:view) {
	    token_names.push_back(token.name);
    }

    std::dynamic_pointer_cast<LR_grammar>(cfg)->parse(token_names,[this](auto const &production)
		    {

		    auto it=all_rules.find(production);
		    if(it==all_rules.end()){
		    return;
		    }
		    auto const &rules=it->second;
		    for(auto const &rule:rules) {
		    for(auto const &argument:rule.arguments) {
		    //if(auto 

		    }
		    }



		    }
		    );


    




}

void S_attributed_SDD::check_dependency() const {
  std::set<attribute_name_type> passed_attributes;
  auto attribute_dependency=get_attribute_dependency();

  const auto check_attribute_dependency =
	  [ &passed_attributes,&attribute_dependency](auto &&self,
                                 const attribute_name_type &attribute) {
	if (attribute.is_terminal()) {
	  return true;
	}
        if (passed_attributes.count(attribute)) {
          return true;
        }
        auto it =attribute_dependency.find(attribute);
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
          std::get<cyy::computation::grammar_symbol_type::nonterminal_type>(
              attribute));
    }
  }
}

} // namespace cyy::compiler
