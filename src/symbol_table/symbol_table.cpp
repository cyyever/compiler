/*!
 * \file symbol_table.hpp
 *
 * \author cyy
 * \date 2018-03-04
 */

#include "symbol_table.hpp"

#include <algorithm>
#include <cassert>
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
    size_t width = 0;
    if (e.type) {
      width = e.type->get_width();
    }
    auto next_address =
        std::max(next_relative_address, e.relative_address + width);
    auto has_insertion =
        symbols.emplace(lexeme, std::make_shared<symbol_entry>(std::move(e)))
            .second;
    if (has_insertion) {
      next_relative_address = next_address;
    }
    return has_insertion;
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
  std::shared_ptr<symbol_table::symbol_entry>
  symbol_table::create_and_get_symbol(const std::string &lexeme) {
    auto ptr = get_symbol(lexeme);
    if (ptr) {
      return ptr;
    }

    symbol_entry e;
    e.lexeme = lexeme;
    add_symbol(std::move(e));
    return get_symbol(lexeme);
  }

  symbol_table::symbol_entry_ptr
  symbol_table::create_temporary_symbol(const std::string &lexeme) {

    symbol_entry e;
    e.lexeme = lexeme;

    auto inserted_symbol = add_symbol(std::move(e));
    assert(inserted_symbol);
    return get_symbol(lexeme);
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
