/*!
 * \file l_attributed_sdd.hpp
 *
 * \brief
 * \author cyy
 * \date 2018-10-28
 */

#pragma once

#include <cyy/computation/contex_free_lang/ll_grammar.hpp>

#include "sdd.hpp"

namespace cyy::compiler {
  class L_attributed_SDD : public SDD {
  public:
    explicit L_attributed_SDD(const LL_grammar &cfg_) : SDD(cfg_) {}

    std::map<std::string, std::any> run(token_span span) const override;

    using SDD::add_inherited_attribute;
    using SDD::add_synthesized_attribute;

  private:
    void check_attributes() const;
  };
} // namespace cyy::compiler
