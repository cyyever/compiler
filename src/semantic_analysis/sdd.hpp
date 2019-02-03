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
#include <optional>
#include <set>
#include <string>
#include <vector>

#include <cyy/computation/contex_free_lang/cfg.hpp>
#include <cyy/computation/lang/lang.hpp>

#include "../token/token.hpp"
#include "grammar_symbol_attribute_name.hpp"

namespace cyy::compiler {
using namespace cyy::computation;

class SDD {
public:
  explicit SDD(const CFG &cfg_) : cfg(cfg_) {}

  virtual ~SDD() = default;

  virtual std::map<grammar_symbol_attribute_name, std::any>
  run(token_span span) = 0;

  struct semantic_rule {
    std::optional<grammar_symbol_attribute_name> result_attribute;
    std::vector<grammar_symbol_attribute_name> arguments;
    using semantic_action_type = std::function<std::optional<std::any>(
        const std::vector<std::reference_wrapper<const std::any>> &)>;
    semantic_action_type action;
  };

protected:
  void add_synthesized_attribute(const CFG::production_type &production,
                                 semantic_rule rule);
  void add_inherited_attribute(const CFG::production_type &production,
                               semantic_rule rule);

private:
  void check_semantic_rule(const CFG::production_type &production,
                           const semantic_rule &rule) const;

protected:
  std::set<grammar_symbol_attribute_name> synthesized_attributes;
  std::set<grammar_symbol_attribute_name> inherited_attributes;
  std::map<grammar_symbol_attribute_name,
           std::set<grammar_symbol_attribute_name>>
      attribute_dependency;
  std::map<grammar_symbol_attribute_name,
           std::set<grammar_symbol_attribute_name>>
      synthesized_attribute_dependency;
  std::map<grammar_symbol_attribute_name,
           std::set<grammar_symbol_attribute_name>>
      inherited_attribute_head_dependency;
  std::map<CFG::production_type, std::vector<semantic_rule>> all_rules;
  const CFG &cfg;
};
} // namespace cyy::compiler
