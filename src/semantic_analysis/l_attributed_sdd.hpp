/*!
 * \file l_attributed_sdd.hpp
 *
 * \brief
 * \author cyy
 * \date 2018-10-28
 */

#pragma once

#include <cyy/computation/contex_free_lang/lr_grammar.hpp>

#include "sdd.hpp"

namespace cyy::compiler {
  class L_attributed_SDD : public SDD {
  public:
    explicit L_attributed_SDD(const LR_grammar &cfg_) : SDD(cfg_) {}

    // std::map<grammar_symbol_attribute_name, std::any> run(token_span span)
    // override;

    using SDD::add_inherited_attribute;
    using SDD::add_synthesized_attribute;

  private:
    void check_inherited_attributes() const;
    // void check_attribute_dependency();
  };
} // namespace cyy::compiler
