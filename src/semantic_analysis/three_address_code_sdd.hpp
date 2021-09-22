/*!
 * \file three_address_code_sdd.hpp
 *
 * \brief
 */

#include "intermediate_representation/three_address_code.hpp"
#include "semantic_analysis/l_attributed_sdd.hpp"
#include "symbol_table/symbol_table.hpp"
#include "token.hpp"

namespace cyy::compiler {

  class three_address_code_SDD {
  public:
    three_address_code_SDD();
    std::optional<std::map<std::string, std::any>>
    run(token_span span,
        const std::unordered_set<std::string> &result_attribute_names) const;

  private:
    std::unique_ptr<L_attributed_SDD> sdd;
    std::shared_ptr<symbol_table> table;
    std::vector<std::shared_ptr<IR::three_address_code::instruction>>
        instruction_sequence;
  };

} // namespace cyy::compiler
