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

namespace cyy::compiler {
using namespace cyy::computation;

class SDD {
public:
  explicit SDD(const CFG &cfg_) : cfg(cfg_) {}

  virtual ~SDD() = default;
  class attribute_name final {


public:

  template <size_t N> 
    attribute_name(const char(&name_)[N]) : name(name_) {}
  attribute_name(std::string_view name_) : name(name_) {}

  const std::string &get_name() const {return name;}

  bool
  belong_to_nonterminal(
                     const grammar_symbol_type::nonterminal_type &nonterminal) const;
  std::optional<size_t> get_terminal_index() const;

private:
  std::string name;
  };

  virtual std::map<attribute_name, std::any> run(token_span span) = 0;

  struct semantic_rule {
    std::optional<attribute_name> result_attribute;
    std::vector<attribute_name> arguments;
    using semantic_action_type = std::function<std::optional<std::any>(
        const std::vector<std::reference_wrapper<const std::any>> &)>;
    semantic_action_type action;
  };

  void add_synthesized_attribute(const CFG::production_type &production,
                                 semantic_rule rule);

protected:
  std::map<CFG::production_type, std::vector<semantic_rule>> all_rules;
  const CFG &cfg;
};
} // namespace cyy::compiler

namespace std {
template <> struct less<cyy::compiler::SDD::attribute_name> {
  bool operator()(const cyy::compiler::SDD::attribute_name &lhs,
                  const cyy::compiler::SDD::attribute_name &rhs) const
noexcept { return lhs.get_name() < rhs.get_name();
  }
};

} // namespace std

