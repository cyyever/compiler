
#include "grammar_symbol_attribute.hpp"

namespace cyy::compiler {

bool grammar_symbol_attribute::belong_nonterminal(
    const name_type &name,
    const grammar_symbol_type::nonterminal_type &nonterminal) {

  if (name.size() <= nonterminal.size()) {
    return false;
  }
  const auto pos = name.find_first_of(nonterminal);

  return pos == 0 && name[nonterminal.size()] == '.';
}

std::optional<size_t>
grammar_symbol_attribute::get_terminal_index(const name_type &name) {

  if (name.size() > 1 && name[0] == '$') {
    size_t index = 0;
    for (size_t i = 1; i < name.size(); i++) {
      if (name[i] < '0' || name[i] > '9') {
        return {};
      }
      index = index * 10 + name[i] - '0';
    }
    if (index == 0) {
      return {};
    }
    return {index};
  }
  return {};
}

} // namespace cyy::compiler
