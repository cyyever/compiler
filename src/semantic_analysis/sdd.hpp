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
//#include "grammar_symbol_attribute.hpp"

namespace cyy::compiler {
using namespace cyy::computation;

class SDD {
public:
  explicit SDD(const CFG & cfg_) : cfg(cfg_) {}

  virtual ~SDD() = default;

  virtual void run(token_string_view view) = 0;

  //using attribute_type = grammar_symbol_attribute;

protected:
  using semantic_action_type = std::function<void(
      std::any &, const std::vector<std::reference_wrapper<const std::any>> &)>;

  struct semantic_rule {
    std::string result_attribute;
    std::vector<std::string> arguments;
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
  std::map<std::string, std::vector<std::string>>
  get_attribute_dependency() const;

  static bool
  belong_nonterminal(const std::string &name,
                     const grammar_symbol_type::nonterminal_type &nonterminal);
  static std::optional<size_t> get_terminal_index(const std::string &name);

protected:
  std::map<CFG::production_type, std::set<semantic_rule, semantic_rule_compare>>
      all_rules;
  const CFG &cfg;
};
} // namespace cyy::compiler
