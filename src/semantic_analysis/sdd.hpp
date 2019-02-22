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

    virtual std::optional<std::map<std::string, std::any>>
    run(token_span span) const = 0;

    struct semantic_rule {
      std::optional<grammar_symbol_attribute_name> result_attribute;
      std::vector<grammar_symbol_attribute_name> arguments;
      using semantic_action_type = std::function<std::optional<std::any>(
          const std::vector<std::reference_wrapper<const std::any>> &)>;
      semantic_action_type action;
    };

  protected:
    void add_synthesized_attribute(const CFG_production &production,
                                   semantic_rule rule);
    void add_inherited_attribute(const CFG_production &production,
                                 semantic_rule rule);

    void resolve_semantic_rules_order() const;

  private:
    void check_semantic_rule(const CFG_production &production,
                             const semantic_rule &rule) const;

  protected:
    mutable std::map<CFG_production, std::vector<semantic_rule>> all_rules;
    std::set<std::string> synthesized_attributes;
    std::set<std::string> inherited_attributes;
    const CFG &cfg;
    mutable bool new_rule_flag{false};
  };
} // namespace cyy::compiler
