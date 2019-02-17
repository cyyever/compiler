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

  std::map<std::string, std::any> L_attributed_SDD::run(token_span span) {
    if (new_rule_flag) {
      check_inherited_attributes();
      resolve_semantic_rules_order();
      new_rule_flag = false;
    }
    if (span.empty()) {
      std::cerr << "span is empty" << std::endl;
      return {};
    }
    return {};
  }

  void L_attributed_SDD::check_inherited_attributes() const {
    for (const auto &[_, rules] : all_rules) {
      assert(!rules.empty());
      for (auto const &rule : rules) {
        // synthesized attribute
        if (!rule.result_attribute) {
          continue;
        }
        auto result_attribute_index = rule.result_attribute->get_index();
        // synthesized attribute
        if (result_attribute_index == 0) {
          continue;
        }

        for (auto const &argument : rule.arguments) {
          auto index = argument.get_index();
          if (index > result_attribute_index) {
            throw exception::no_inherited_grammar_symbol_attribute(
                rule.result_attribute->get_name());
          }

          // head's attribute must be synthesize
          if (index == 0) {
            if (!std::any_of(rules.begin(), rules.end(),
                             [&argument](const auto &r) {
                               return r.result_attribute &&
                                      r.result_attribute->get_suffix() ==
                                          argument.get_suffix();
                             })) {

              throw exception::no_synthesized_grammar_symbol_attribute(
                  argument.get_name());
            }
          }
        }
      }
    }
  }
} // namespace cyy::compiler
