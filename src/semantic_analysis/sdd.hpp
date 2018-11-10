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
#include "grammar_symbol_attribute.hpp"

namespace cyy::compiler {
using namespace cyy::computation;

class SDD {
public:
  explicit SDD(std::shared_ptr<CFG> cfg_) : cfg(cfg_) {}

  virtual ~SDD() = default;

  virtual void run(token_string_view view) = 0;

  using attribute_type = grammar_symbol_attribute;

protected:
  using semantic_action_type = std::function<void(
      attribute_type::value_type &, const std::vector<std::reference_wrapper<
                                        const attribute_type::value_type>> &)>;

  struct semantic_rule {
    attribute_type::name_type result_attribute;
    std::vector<attribute_type::name_type> arguments;
    semantic_action_type action;
  };

  struct semantic_rule_compare {
    bool operator()(const cyy::compiler::SDD::semantic_rule &lhs,
                    const cyy::compiler::SDD::semantic_rule &rhs) const
        noexcept {
      return lhs.result_attribute < rhs.result_attribute;
    }
  };

  void add_synthesized_attribute(const CFG::production_type &production,
                                 semantic_rule rule);
  std::map<attribute_type::name_type, std::vector<attribute_type::name_type>>
  get_attribute_dependency() const;

protected:
  std::map<attribute_type::name_type, attribute_type::value_type>
      all_attributes;
  std::map<CFG::production_type, std::set<semantic_rule, semantic_rule_compare>>
      all_rules;
  const std::shared_ptr<CFG> cfg;
};
} // namespace cyy::compiler
