/*!
 * \file sdd.hpp
 *
 * \author cyy
 * \date 2018-03-04
 */

#pragma once

#include <any>
#include <optional>
#include <string>
#include <string_view>

#include <cyy/computation/formal_grammar/grammar.hpp>

namespace cyy::compiler {
using namespace cyy::computation;
class grammar_symbol_attribute final {
public:
  using name_type = std::string;
  using value_type = std::any;

  grammar_symbol_attribute(std::string_view name_) : name(name_) {}

  const std::string &get_name() const { return name; }

  static bool
  belong_nonterminal(const name_type &name,
                     const grammar_symbol_type::nonterminal_type &nonterminal);
  static std::optional<size_t> get_terminal_index(const name_type &name);

private:
  name_type name;
  value_type value;
};
} // namespace cyy::compiler

namespace std {
template <> struct less<cyy::compiler::grammar_symbol_attribute> {
  bool operator()(const cyy::compiler::grammar_symbol_attribute &lhs,
                  const cyy::compiler::grammar_symbol_attribute &rhs) const
      noexcept {
    return lhs.get_name() < rhs.get_name();
  }
};

} // namespace std
