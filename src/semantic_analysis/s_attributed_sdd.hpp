/*!
 * \file s_attributed_sdd.hpp
 *
 * \brief
 * \author cyy
 * \date 2018-10-28
 */

#pragma once

#include "sdd.hpp"
#include <cyy/computation/contex_free_lang/lr_grammar.hpp>

namespace cyy::compiler {
  class S_attributed_SDD : public SDD {
  public:
    explicit S_attributed_SDD(const LR_grammar &cfg_) : SDD(cfg_) {}

    std::map<std::string, std::any> run(token_span span) override;

    using SDD::add_synthesized_attribute;

  private:
    void check_attribute_dependency();
    const std::vector<semantic_rule> &
    get_semantic_rules(const cyy::computation::CFG_production &production);
  };
} // namespace cyy::compiler
