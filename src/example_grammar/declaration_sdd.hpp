/*!
 * \file declaration_sdd.hpp
 *
 * \brief
 */
#pragma once
#include <cyy/computation/context_free_lang/ll_grammar.hpp>

#include "semantic_analysis/l_attributed_sdd.hpp"
#include "symbol_table/symbol_table.hpp"
#include "token.hpp"

namespace cyy::compiler::example_grammar {

  class declaration_SDD {
  public:
    declaration_SDD();
    bool run(token_span span);

    auto get_symbol_table() const { return table; }

  private:
    std::unique_ptr<L_attributed_SDD> sdd;
    std::shared_ptr<symbol_table> table;
    std::shared_ptr<LL_grammar> grammar;
  };

} // namespace cyy::compiler::example_grammar
