/*!
 * \file l_attributed_sdd.hpp
 */

#pragma once

#include <cyy/computation/context_free_lang/ll_grammar.hpp>

#include "sdd.hpp"

namespace cyy::compiler {
  class L_attributed_SDD : public SDD {
  public:
    explicit L_attributed_SDD(const LL_grammar &cfg_) : SDD(cfg_) {}

    using SDD::add_inherited_attribute;
    using SDD::add_synthesized_attribute;

  private:
    std::optional<std::map<std::string, std::any>>
    _run(token_span span,
         const std::unordered_set<std::string> &result_attribute_names)
        const override;
    void check_attributes() const;
  };
} // namespace cyy::compiler
