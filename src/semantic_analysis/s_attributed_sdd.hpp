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
  explicit S_attributed_SDD(std::shared_ptr<LR_grammar> cfg_) : SDD(cfg_) {}

  void run(token_string_view view) override;

private:
  void check_dependency() const;
};
} // namespace cyy::compiler
