/*!
 * \file exception.hpp
 *
 * \author cyy
 * \date 2018-03-03
 */

#pragma once

#include <stdexcept>

namespace cyy::compiler::exception {

  class io_error : public std::runtime_error {
  public:
    using runtime_error::runtime_error;
  };

  class invalid_semantic_rule : public std::invalid_argument {
  public:
    using invalid_argument::invalid_argument;
  };

  class unexisted_production : public std::invalid_argument {
  public:
    using invalid_argument::invalid_argument;
  };

  class semantic_rule_confliction : public std::invalid_argument {
  public:
    using invalid_argument::invalid_argument;
  };

  class invalid_grammar_symbol_attribute_name : public std::invalid_argument {
  public:
    using invalid_argument::invalid_argument;
  };

  class unexisted_grammar_symbol_attribute : public std::invalid_argument {
  public:
    using invalid_argument::invalid_argument;
  };

  class no_synthesized_grammar_symbol_attribute : public std::invalid_argument {
  public:
    using invalid_argument::invalid_argument;
  };

  class no_inherited_grammar_symbol_attribute : public std::invalid_argument {
  public:
    using invalid_argument::invalid_argument;
  };

  class grammar_symbol_attribute_dependency_circle
      : public std::invalid_argument {
  public:
    using invalid_argument::invalid_argument;
  };

  class type_name_confliction : public std::invalid_argument {
  public:
    using invalid_argument::invalid_argument;
  };

  class existed_symbol_table_entry : public std::invalid_argument {
  public:
    using invalid_argument::invalid_argument;
  };
} // namespace cyy::compiler::exception
