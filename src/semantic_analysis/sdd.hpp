/*!
 * \file sdd.hpp
 *
 * \author cyy
 * \date 2018-03-04
 */

#pragma once

#include <any>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <cyy/computation/contex_free_lang/cfg.hpp>
#include <cyy/computation/lang/lang.hpp>

namespace cyy::compiler {
using namespace cyy::computation;
class SDD {
public:
  using attribute_name_type = cyy::computation::grammar_symbol_type;
  using attribute_value_type = std::any;
  using semantic_action_type = std::function<void(
      attribute_value_type &, const std::vector<attribute_value_type &> &)>;

  struct semantic_rule {
    attribute_name_type attribute;
    std::vector<attribute_name_type> arguments;
    semantic_action_type action;
  };

public:
  explicit SDD(std::shared_ptr<CFG> cfg_) : cfg(cfg_) {}

protected:
  void add_synthesized_attribute(const CFG::production_type &production,
                                 const semantic_rule &rule);
  static bool
  is_attribute_of_grammar_symbol(const grammar_symbol_type &grammar_symbol,
                                 const attribute_name_type &attribute_name) {
    auto nonterminal_ptr = grammar_symbol.get_nonterminal_ptr();
    if (nonterminal_ptr) {
      auto name_ptr = attribute_name.get_nonterminal_ptr();
      if (!name_ptr) {
        return false;
      }
      if (name_ptr->size() <= nonterminal_ptr->size()) {
        return false;
      }
      const auto pos = name_ptr->find_first_of(*nonterminal_ptr);

      return pos != std::string::npos &&
             (*name_ptr)[nonterminal_ptr->size()] == '.';
    }
    return grammar_symbol == attribute_name;
  }

protected:
  std::map<attribute_name_type, attribute_value_type> all_attributes;

private:
  const std::shared_ptr<CFG> cfg;
};
} // namespace cyy::compiler
