/*!
 * \file sdd.hpp
 *
 * \author cyy
 * \date 2018-03-04
 */

#pragma once

#include <string>
#include <vector>
#include <memory>

#include <cyy/computation/cfg.hpp>
#include <cyy/computation/lang.hpp>

namespace cyy::compiler {

class  SDD {
public:
  struct attribute {
    using value_type=std::shared<void>;
    std::string name;
    value_type value;
  }
public:
  explicit SDD()=default;

  void add_semantic_rule(                );

private:
  std::unique_ptr<CFG> cfg;
};
} 
