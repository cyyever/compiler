
/*!
 * \file operator.hpp
 *
 * \author cyy
 */

#pragma once
namespace cyy::compiler {
  enum class unary_arithmetic_operator {
    minus = 0,
  };

  enum class unary_logical_operator {
    negation = 10,
  };

  enum class binary_arithmetic_operator : int {
    addition = 100,
    subtraction = 101,
    multiplication = 102,
  };

  enum class binary_logical_operator : int {
    AND = 110,
    OR = 111,
  };

  enum class relational_operator : int {
    equal = 120,
    unequal = 121,
    greater = 122,
    greater_or_equal = 123,
    less = 124,
    less_or_equal = 125,
  };

} // namespace cyy::compiler