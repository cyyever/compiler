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
#include "l_attributed_sdd.hpp"

namespace cyy::compiler {

  void L_attributed_SDD::check_attributes() const {
    /*
    for (auto const &attribute : inherited_attributes) {
      auto it = attribute_dependency.find(attribute);
      if (it == attribute_dependency.end() || it->second.empty()) {
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
    */
  }
} // namespace cyy::compiler
