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
  bool symbol_table::has_entry(const std::string &lexeme) const {
    if (entries.contains(lexeme)) {
      return true;
    }

    if (prev_table) {
      return prev_table->has_entry(lexeme);
    }
    return false;
  }
  bool symbol_table::add_entry(symbol_table_entry e) {
    auto lexeme = e.lexeme;
    return entries
        .emplace(lexeme, std::make_shared<symbol_table_entry>(std::move(e)))
        .second;
  }
  std::shared_ptr<symbol_table_entry>
  symbol_table::get_entry(const std::string &lexeme) const {
    auto it = entries.find(lexeme);
    if (it != entries.end()) {
      return it->second;
    }
    if (prev_table) {
      return prev_table->get_entry(lexeme);
    }
    return {};
  }
  bool symbol_table::add_type(
      std::shared_ptr<type_expression::type_name> expr,
      std::shared_ptr<symbol_table> associated_symbol_table) {
    return types
        .emplace(expr->get_name(), std::pair{expr, associated_symbol_table})

        .second;
  }
  std::optional<std::pair<std::shared_ptr<type_expression::type_name>,
                          std::shared_ptr<symbol_table>>>
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
