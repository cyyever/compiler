/*!
 * \file constant_table.hpp
 *
 * \author cyy
 * \date 2018-03-04
 */

#include "constant_table.hpp"

namespace cyy::compiler {
  bool constant_table::add_constant(std::string lexeme) {
    entry e;
    e.lexeme = lexeme;
    auto has_insertion =
        constants.emplace(lexeme, std::make_shared<entry>(std::move(e))).second;
    return has_insertion;
  }
  constant_table::entry_ptr
  constant_table::get_constant(const std::string &lexeme) const {
    auto it = constants.find(lexeme);
    if (it != constants.end()) {
      return it->second;
    }
    return {};
  }

} // namespace cyy::compiler
