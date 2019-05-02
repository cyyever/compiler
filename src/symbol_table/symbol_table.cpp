/*!
 * \file symbol_table.hpp
 *
 * \author cyy
 * \date 2018-03-04
 */

#include "symbol_table.hpp"

namespace cyy::compiler {
  bool symbol_table::has_entry(const std::string &lexeme) const {
    symbol_table_entry e;
    e.lexeme = lexeme;

    auto it = entries.find(e);
    if (it != entries.end()) {
      return true;
    }
    if (prev_table) {
      return prev_table->has_entry(lexeme);
    }
    return false;
  }
  bool symbol_table::add_entry(const symbol_table_entry &e) {
    return entries.emplace(e).second;
  }
  std::optional<symbol_table_entry>
  symbol_table::get_entry(const std::string &lexeme) const {
    symbol_table_entry e;
    e.lexeme = lexeme;
    auto it = entries.find(e);
    if (it != entries.end()) {
      return *it;
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
