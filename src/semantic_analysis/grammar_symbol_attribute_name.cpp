
#include "grammar_symbol_attribute_name.hpp"
#include "../exception.hpp"

namespace cyy::compiler {

  grammar_symbol_attribute_name::grammar_symbol_attribute_name(std::string_view name_) : name(name_) {
    if(name.empty() || name[0] != '$') {
      throw  exception::invalid_grammar_symbol_attribute_name(name);
    }

    index = 0;
    for (size_t i = 1; i < name.size(); i++) {
      if(name[i]=='.') {
        if(i==1 || i+1==name.size()) {
          throw  exception::invalid_grammar_symbol_attribute_name(name);
        }
        suffix=name.substr(i+1);
        break;
      }
      if (name[i] < '0' || name[i] > '9') {
        throw  exception::invalid_grammar_symbol_attribute_name(name);
      }
      index = index * 10 + name[i] - '0';
    }
  }
bool grammar_symbol_attribute_name::belong_to_nonterminal(
    const cyy::computation::grammar_symbol_type::nonterminal_type &nonterminal)
    const {

  if (name.size() <= nonterminal.size()) {
    return false;
  }
  const auto pos = name.find_first_of(nonterminal);
  return pos == 0 && name[nonterminal.size()] == '.';
}

std::optional<size_t>
grammar_symbol_attribute_name::get_terminal_index() const {
  /*
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
  */
  return {};
}

} // namespace cyy::compiler
