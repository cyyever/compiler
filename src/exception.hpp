/*!
 * \file exception.hpp
 *
 * \author cyy
 * \date 2018-03-03
 */

#pragma once

#include <stdexcept>

namespace cyy::compiler::exception {

class io_error: public std::runtime_error {
public:
  using runtime_error::runtime_error;
};

class  invalid_semantic_rule: public std::invalid_argument {
public:
  using invalid_argument::invalid_argument;
};

class unexisted_production : public std::invalid_argument {
public:
  using invalid_argument::invalid_argument;
};

class semantic_rule_confliction: public std::invalid_argument {
public:
  using invalid_argument::invalid_argument;
};

class unexisted_grammar_symbol_attribute : public std::invalid_argument {
public:
  using invalid_argument::invalid_argument;
};

class orphan_grammar_symbol_attribute : public std::invalid_argument {
public:
  using invalid_argument::invalid_argument;
};

class no_synthesized_grammar_symbol_attribute : public std::invalid_argument {
public:
  using invalid_argument::invalid_argument;
};

} // namespace cyy::compiler::exception
