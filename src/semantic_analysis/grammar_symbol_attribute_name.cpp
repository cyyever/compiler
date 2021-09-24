
#include "grammar_symbol_attribute_name.hpp"

#include "../exception.hpp"

namespace cyy::compiler {

  grammar_symbol_attribute_name::grammar_symbol_attribute_name(
      std::string_view name_)
      : name(name_) {
    if (name.empty() || name[0] != '$') {
      throw exception::invalid_grammar_symbol_attribute_name(name);
    }

    index = 0;
    for (size_t i = 1; i < name.size(); i++) {
      if (name[i] == '.') {
        if (i == 1 || i + 1 == name.size()) {
          throw exception::invalid_grammar_symbol_attribute_name(name);
        }
        suffix = name.substr(i + 1);
        break;
      }
      if (name[i] < '0' || name[i] > '9') {
        throw exception::invalid_grammar_symbol_attribute_name(name);
      }
      index = index * 10 + name[i] - '0';
    }
    if (index == 0 && suffix.empty()) {
      throw exception::unexisted_grammar_symbol_attribute(name);
    }
  }
  bool grammar_symbol_attribute_name::belong_to_nonterminal(
      const cyy::computation::grammar_symbol_type::nonterminal_type
          &nonterminal) const {

    if (name.size() <= nonterminal.size()) {
      return false;
    }
    const auto pos = name.find_first_of(nonterminal);
    return pos == 0 && name[nonterminal.size()] == '.';
  }

  bool grammar_symbol_attribute_name::belong_to_production(
      const cyy::computation::CFG_production &production) const {
    if (index == 0) {
      return match(production.get_head());
    }

    if (index > production.get_body().size()) {
      return false;
    }

    auto const &grammar_symbol = production.get_body()[index - 1];
    return match(grammar_symbol);
  }

  std::string grammar_symbol_attribute_name::get_full_name(
      const cyy::computation::CFG_production &production) const {
    if (index == 0) {
      return production.get_head() + "." + suffix;
    }
    if (index > production.get_body().size()) {
      throw exception::unexisted_grammar_symbol_attribute(name);
    }

    auto const &grammar_symbol = production.get_body()[index - 1];
    if (!match(grammar_symbol)) {
      throw exception::unexisted_grammar_symbol_attribute(name);
    }
    auto ptr = grammar_symbol.get_nonterminal_ptr();
    if (ptr) {
      return *ptr + "." + suffix;
    }
    return "token";
  }
  bool grammar_symbol_attribute_name::match(
      const cyy::computation::grammar_symbol_type &grammar_symbol) const {
    bool is_nonterminal = grammar_symbol.is_nonterminal();
    return is_nonterminal != suffix.empty();
  }

} // namespace cyy::compiler
