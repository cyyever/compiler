/*!
 * \file s_attributed_sdd.cpp
 *
 * \brief
 * \author cyy
 * \date 2018-10-28
 */

#include "s_attributed_sdd.hpp"
#include "../exception.hpp"

namespace cyy::compiler {

void S_attributed_SDD::run() {}

void S_attributed_SDD::check_dependency() const {
  /*
  std::set<const attribute_name_type *> passed_attributes;
  ;

  const auto check_attribute_dependency =
      [this, &passed_attributes](auto &&self,
                                 const attribute_name_type &attribute) {
	if (attribute.is_terminal()) {
	  return true;
	}
        if (passed_attributes.count(&attribute)) {
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
        passed_attributes.insert(&attribute);
        return true;
      };

  for (const auto &[attribute, _] : attribute_dependency) {
    if (!check_attribute_dependency(check_attribute_dependency, attribute)) {
      throw cyy::compiler::exception::orphan_grammar_symbol_attribute(
          std::get<cyy::computation::grammar_symbol_type::nonterminal_type>(
              attribute));
    }
  }
  */
}

} // namespace cyy::compiler
