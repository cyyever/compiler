
#include "grammar_symbol_attribute_name.hpp"
#include <iostream>

namespace cyy::compiler {

bool grammar_symbol_attribute_name::belong_to_nonterminal(
    const cyy::computation::grammar_symbol_type::nonterminal_type &nonterminal) const {
  return true;

  if (name.size() <= nonterminal.size()) {
    return false;
  }
  const auto pos = name.find_first_of(nonterminal);
  std::cout<<"pos="<<pos<<std::endl;
  std::cout<<"name="<<name <<" nonterminal="<<nonterminal<<std::endl;

  return pos == 0 && name[nonterminal.size()] == '.';
}

std::optional<size_t>
grammar_symbol_attribute_name::get_terminal_index() const {
  if (name.size() > 1 && name[0] == '$') {
    size_t index = 0;
    for (size_t i = 1; i < name.size(); i++) {
      if (name[i] < '0' || name[i] > '9') {
        return {};
      }
      index = index * 10 + name[i] - '0';
    }
    return {index};
  }
  return {};
}

} // namespace cyy::compiler
