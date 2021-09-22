/*!
 * \file three_address_code_sdd.hpp
 *
 * \brief
 */

#include "intermediate_representation/three_address_code.hpp"
#include "semantic_analysis/s_attributed_sdd.hpp"
#include "symbol_table/symbol_table.hpp"
#include "token.hpp"

namespace cyy::compiler {

  class three_address_code_SDD {
  public:
    three_address_code_SDD();
    bool run(token_span span, std::shared_ptr<symbol_table> table_ = {});

  public:
    std::vector<std::shared_ptr<IR::three_address_code::instruction>>
        instruction_sequence;

  private:
    std::unique_ptr<S_attributed_SDD> sdd;
    std::shared_ptr<symbol_table> table;
  };

} // namespace cyy::compiler
