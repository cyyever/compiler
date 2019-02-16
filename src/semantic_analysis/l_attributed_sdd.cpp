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
  void L_attributed_SDD::check_inherited_attributes() const {
    for (auto &[_, rules] : all_rules) {
      assert(!rules.empty());
      for (auto const &rule : rules) {
        if (!rule.result_attribute) {
          continue;
        }
        auto result_attribute_index = rule.result_attribute->get_index();

        for (auto const &argument : rule.arguments) {
          if (argument.get_index() > result_attribute_index) {
            throw exception::no_inherited_grammar_symbol_attribute(
                rule.result_attribute->get_name());
          }
        }
      }
    }
  }
} // namespace cyy::compiler
