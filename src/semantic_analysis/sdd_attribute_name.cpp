
#include "sdd_attribute_name.hpp"

namespace cyy::compiler {

bool SDD::attribute_name::belong_to_nonterminal(
    const grammar_symbol_type::nonterminal_type &nonterminal) const {

  if (name.size() <= nonterminal.size()) {
    return false;
  }
  const auto pos = name.find_first_of(nonterminal);

  return pos == 0 && name[nonterminal.size()] == '.';
}

std::optional<size_t>
SDD::attribute_name::get_terminal_index() const {
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
