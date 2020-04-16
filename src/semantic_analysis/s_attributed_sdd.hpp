/*!
 * \file s_attributed_sdd.hpp
 *
 * \brief
 * \author cyy
 * \date 2018-10-28
 */

#pragma once

#include <cyy/computation/context_free_lang/lr_grammar.hpp>

#include "sdd.hpp"

namespace cyy::compiler {
  class S_attributed_SDD : public SDD {
  public:
    explicit S_attributed_SDD(const LR_grammar &cfg_) : SDD(cfg_) {}

    using SDD::add_synthesized_attribute;

  private:
    std::optional<std::map<std::string, std::any>>
    _run(token_span span,
         const std::unordered_set<std::string> &result_attribute_names)
        const override;
    void check_attributes() const;
  };
} // namespace cyy::compiler
