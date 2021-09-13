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

#include <cyy/computation/context_free_lang/cfg_production.hpp>
#include <cyy/computation/formal_grammar/grammar_symbol.hpp>

namespace cyy::compiler {
  class grammar_symbol_attribute_name final {

  public:
    template <size_t N>
    grammar_symbol_attribute_name(const char (&name_)[N])
        : grammar_symbol_attribute_name(std::string_view(name_)) {}
    grammar_symbol_attribute_name(std::string_view name_);

    const std::string &get_name() const { return name; }
    auto operator<=>(const grammar_symbol_attribute_name & rhs)  const {
      return get_name()<=>rhs.get_name();

    }

    std::string
    get_full_name(const cyy::computation::CFG_production &production) const;

    bool belong_to_nonterminal(
        const cyy::computation::grammar_symbol_type::nonterminal_type
            &nonterminal) const;

    bool belong_to_nonterminal() const { return !suffix.empty(); }

    bool belong_to_production(
        const cyy::computation::CFG_production &production) const;

    size_t get_index() const { return index; }

    const std::string &get_suffix() const { return suffix; }

    bool operator==(const grammar_symbol_attribute_name &rhs) const {
      return name == rhs.name;
    }

    bool
    match(const cyy::computation::grammar_symbol_type &grammar_symbol) const;

  private:
    std::string name;
    size_t index{};
    std::string suffix;
  };
} // namespace cyy::compiler

