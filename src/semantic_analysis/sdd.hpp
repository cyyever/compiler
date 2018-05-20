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
#include <functional>

#include <cyy/computation/cfg.hpp>
#include <cyy/computation/lang.hpp>

namespace cyy::compiler {
  using namespace cyy::computation;
class  SDD {
public:
  struct attribute {
    using value_type=std::shared_ptr<void>;
    std::string name;
    value_type value;
  };

  struct semantic_rule {
      std::string attr_name,
      std::vector<std::string> argument_attr_names
      std::function< attribute::value_type( const    std::vector<attribute::value_type>   &   )  > action;
  };
public:
  explicit SDD()=default;

  void add_synthesized_attribute( 
      const CFG::production_type & production ,
      const std::string & attr_name ,
      const std::vector<std::string> &argument_attr_names
      , std::function< attribute::value_type( const    std::vector<attribute::value_type>   &  )  >
     );


  //std::map<production_type>

private:
  std::unique_ptr<CFG> cfg;
};
} 
