/*!
 * \file s_attributed_sdd.hpp
 *
 * \brief
 * \author cyy
 * \date 2018-10-28
 */

#pragma once

#include "sdd.hpp"

namespace cyy::compiler {
class S_attributed_SDD : public SDD {
public:
  explicit S_attributed_SDD(std::shared_ptr<CFG> cfg_) : SDD(cfg_) {}
  using SDD::add_synthesized_attribute;

  void run() override;

private:
  void check_dependency() const;
};
} // namespace cyy::compiler
