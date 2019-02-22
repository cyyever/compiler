/*!
 * \file sdd.hpp
 *
 * \author cyy
 * \date 2018-03-04
 */

#pragma once

#include <set>

#include <cyy/computation/lang/lang.hpp>

#include "../token/token.hpp"

namespace cyy::compiler {
  using namespace cyy::computation;

  class symbol_table final {
    private
     struct entry {
       symbol_string lexeme;
     };
  public:
    symbol_table()=default;

    ~symbol_table() = default;

  private:
    std::unordered_set<entry> table;
  };
} // namespace cyy::compiler
