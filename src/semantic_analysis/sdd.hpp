/*!
 * \file sdd.hpp
 *
 * \author cyy
 * \date 2018-03-04
 */

#pragma once

#include <any>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <cyy/computation/contex_free_lang/cfg.hpp>
#include <cyy/computation/lang/lang.hpp>

#include "../token/token.hpp"
namespace cyy::compiler {
using namespace cyy::computation;
class SDD {
public:
  using attribute_name_type = std::string;
  using attribute_value_type = std::any;
  using semantic_action_type = std::function<void(
      attribute_value_type &,
      const std::vector<std::reference_wrapper<const attribute_value_type>> &)>;

  struct semantic_rule {
    attribute_name_type attribute;
    std::vector<attribute_name_type> arguments;
    semantic_action_type action;
  };

public:
  explicit SDD(std::shared_ptr<CFG> cfg_) : cfg(cfg_) {}

  virtual ~SDD() = default;

  virtual void run(token_string_view view) = 0;

protected:
  void add_synthesized_attribute(const CFG::production_type &production,
                                 semantic_rule rule);
  std::map<attribute_name_type, std::vector<attribute_name_type>>
  get_attribute_dependency() const;
  static bool is_attribute_of_nonterminal(
      const grammar_symbol_type::nonterminal_type &nonterminal,
      const attribute_name_type &attribute_name);

protected:
  std::map<attribute_name_type, attribute_value_type> all_attributes;
  std::map<CFG::production_type, std::set<semantic_rule>> all_rules;
  const std::shared_ptr<CFG> cfg;
};
} // namespace cyy::compiler
