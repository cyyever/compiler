/*!
 * \file declaration_sdd.hpp
 *
 * \brief
 */
#pragma once

#include "semantic_analysis/l_attributed_sdd.hpp"
#include "symbol_table/symbol_table.hpp"
#include "token.hpp"

namespace cyy::compiler::example_grammar {

  class declaration_SDD {
  public:
    declaration_SDD();
    std::shared_ptr<symbol_table> run(token_span span);

  private:
    std::unique_ptr<L_attributed_SDD> sdd;
    std::shared_ptr<LL_grammar> grammar;
  };

} // namespace cyy::compiler::example_grammar
