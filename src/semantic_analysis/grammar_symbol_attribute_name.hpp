/*!
 * \file sdd.hpp
 *
 * \author cyy
 * \date 2018-03-04
 */

#pragma once

#include <optional>
#include <string>
#include <string_view>

#include <cyy/computation/formal_grammar/grammar.hpp>

namespace cyy::compiler {
class grammar_symbol_attribute_name final {

public:
  template <size_t N>
  grammar_symbol_attribute_name(const char (&name_)[N]) : name(name_) {}
  grammar_symbol_attribute_name(std::string_view name_) : name(name_) {}

  const std::string &get_name() const { return name; }

  bool belong_to_nonterminal(
      const cyy::computation::grammar_symbol_type::nonterminal_type
          &nonterminal) const;
  std::optional<size_t> get_terminal_index() const;

  bool operator==(const grammar_symbol_attribute_name &rhs) const {
    return name == rhs.name;
  }

private:
  std::string name;
};
} // namespace cyy::compiler

namespace std {
template <> struct less<cyy::compiler::grammar_symbol_attribute_name> {
  bool operator()(const cyy::compiler::grammar_symbol_attribute_name &lhs,
                  const cyy::compiler::grammar_symbol_attribute_name &rhs) const
      noexcept {
    return lhs.get_name() < rhs.get_name();
  }
};

} // namespace std
