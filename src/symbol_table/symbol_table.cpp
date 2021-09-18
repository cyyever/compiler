/*!
 * \file symbol_table.hpp
 *
 * \author cyy
 * \date 2018-03-04
 */

#include "symbol_table.hpp"
#include <algorithm>
#include <iostream>

namespace cyy::compiler {
  bool symbol_table::has_symbol(const std::string &lexeme) const {
    if (symbols.contains(lexeme)) {
      return true;
    }

    if (prev_table) {
      return prev_table->has_symbol(lexeme);
    }
    return false;
  }
  bool symbol_table::add_symbol(symbol_entry e) {
    auto lexeme = e.lexeme;
    return symbols.emplace(lexeme, std::make_shared<symbol_entry>(std::move(e)))
        .second;
  }
  std::shared_ptr<symbol_table::symbol_entry>
  symbol_table::get_symbol(const std::string &lexeme) const {
    auto it = symbols.find(lexeme);
    if (it != symbols.end()) {
      return it->second;
    }
    if (prev_table) {
      return prev_table->get_symbol(lexeme);
    }
    return {};
  }
  bool symbol_table::add_type(type_entry t) {
    auto lexeme = t.type->get_name();
    return types.emplace(lexeme, std::make_shared<type_entry>(std::move(t)))
        .second;
  }
  std::shared_ptr<symbol_table::type_entry>
  symbol_table::get_type(const std::string &type_name) const {
    auto it = types.find(type_name);
    if (it != types.end()) {
      return it->second;
    }
    if (prev_table) {
      return prev_table->get_type(type_name);
    }
    return {};
  }
} // namespace cyy::compiler
