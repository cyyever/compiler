
/*!
 * \file token.hpp
 *
 * \author cyy
 */

#pragma once
namespace cyy::compiler{
  enum class binary_arithmetic_operator {
    addition,
    subtraction,
    multiplication,
  };

  enum class unary_arithmetic_operator {
    minus,
  };

  enum class unary_logical_operator {
    negation,
  };

  enum class binary_logical_operator {
    AND,
    OR,
  };

  enum class relational_operator {
    equal,
    unequal,
    greater,
    greater_or_equal,
    less,
    less_or_equal,
  };

} // namespace cyy::compiler::operator
